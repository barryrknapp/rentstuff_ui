#include "hookapi.h"

// Constants
#define SYSTEM_WALLET "rSpreadHookAddress..." // Replace: Spread Hook account (from setup_hook_fse_burn.js)
#define RLUSD_ISSUER "rRLUSD..."              // Replace: Actual RLUSD issuer
#define ESCROW_TIMEOUT 604800                 // 7 days
#define STATE_KEY_SIZE 32
#define MEMO_SIZE 256
#define MIN_XRP 0.000001 // 1 drop
#define MAX_SLIPPAGE 200 // 2% slippage

// Debug log
#define DEBUG_LOG(msg, val)                           \
    do                                                \
    {                                                 \
        uint8_t log[64];                              \
        uint32_t log_len = 64;                        \
        uint64_to_string(log, &log_len, val);         \
        trace(msg, sizeof(msg) - 1, log, log_len, 0); \
    } while (0)

// Utility: Parse memo for refund_to, pay_to, id
int64_t parse_memo(uint8_t *memo, uint32_t memo_len, uint8_t *refund_to, uint8_t *pay_to, uint8_t *id)
{
    uint8_t *ptr = memo;
    uint32_t i = 0;
    // Simple JSON-like parsing (assumes {"refund_to":"r...","pay_to":"r...","id":"123"})
    if (memo_len < 50)
        return -1; // Too short
    // Skip to refund_to
    while (i < memo_len && ptr[i++] != ':')
    {
    }
    if (i >= memo_len || ptr[i] != '"')
        return -1;
    i++; // Skip quote
    for (int j = 0; j < 34 && i < memo_len; j++)
        refund_to[j] = ptr[i++];
    refund_to[34] = 0;
    // Skip to pay_to
    while (i < memo_len && ptr[i++] != ':')
    {
    }
    if (i >= memo_len || ptr[i] != '"')
        return -1;
    i++;
    for (int j = 0; j < 34 && i < memo_len; j++)
        pay_to[j] = ptr[i++];
    pay_to[34] = 0;
    // Skip to id
    while (i < memo_len && ptr[i++] != ':')
    {
    }
    if (i >= memo_len || ptr[i] != '"')
        return -1;
    i++;
    for (int j = 0; j < 32 && i < memo_len; j++)
        id[j] = ptr[i++];
    id[32] = 0;
    return 0;
}

// Utility: Get DEX XRP/RLUSD rate (mid-price from AMM)
int64_t get_dex_rate(uint8_t *currency, uint8_t *issuer)
{
    uint8_t amm_info[256];
    uint32_t amm_len = 256;
    if (ledger_amm_info(amm_info, &amm_len, currency, issuer, XRP_CURRENCY, XRP_ISSUER) != OK)
    {
        return -1; // AMM not found
    }
    // Parse AMM state for mid-price (simplified, assumes 1 XRP = N RLUSD)
    uint64_t xrp_pool, rlusd_pool;
    GUARD(32);
    xrp_pool = INT64_FROM_BUF(amm_info + 20);   // AMM pool XRP balance
    rlusd_pool = INT64_FROM_BUF(amm_info + 28); // AMM pool RLUSD balance
    if (xrp_pool == 0)
        return -1;
    return (rlusd_pool * 1000000) / xrp_pool; // RLUSD per XRP (drops)
}

// Hook: Main entry
int64_t hook(uint32_t reserved)
{
    // Get transaction type
    uint8_t tx_type[8];
    GUARD(32);
    if (hook_tx_type(tx_type, 8) != 8)
        return -1;

    // Handle Payment (renter payment)
    if (BUF_COMPARE(tx_type, "Payment", 7) == 0)
    {
        // Get amount (XRP drops)
        uint64_t xrp_drops = hook_tx_amount();
        if (xrp_drops < MIN_XRP * 1000000)
            return -2; // Too small

        // Get memo
        uint8_t memo[MEMO_SIZE], refund_to[35], pay_to[35], id[33];
        uint32_t memo_len = MEMO_SIZE;
        if (hook_tx_memo(memo, &memo_len) != OK || parse_memo(memo, memo_len, refund_to, pay_to, id) != 0)
        {
            return -3; // Invalid memo
        }

        // Split 50% to RLUSD via DEX
        uint64_t xrp_to_sell = xrp_drops / 2;
        uint8_t amm_tx[256];
        uint32_t amm_len = 256;
        uint64_t rlusd_drops = get_dex_rate(RLUSD_CURRENCY, RLUSD_ISSUER);
        if (rlusd_drops == -1)
            return -4; // AMM fetch failed
        uint64_t rlusd_out = (xrp_to_sell * rlusd_drops) / 1000000;
        if (amm_len = prepare_amm_trade(amm_tx, &amm_len, xrp_to_sell, RLUSD_CURRENCY, RLUSD_ISSUER, rlusd_out, MAX_SLIPPAGE) != OK)
        {
            return -5; // AMM trade prep failed
        }
        emit_tx(amm_tx, amm_len);

        // Create escrow
        uint8_t escrow_tx[256];
        uint32_t escrow_len = 256;
        uint64_t remaining_xrp = xrp_drops - xrp_to_sell;
        if (prepare_escrow_create(escrow_tx, &escrow_len, remaining_xrp, RLUSD_CURRENCY, RLUSD_ISSUER, rlusd_out, ESCROW_TIMEOUT, memo, memo_len) != OK)
        {
            return -6; // Escrow prep failed
        }
        emit_tx(escrow_tx, escrow_len);

        // Store memo data in state
        uint8_t state_key[STATE_KEY_SIZE];
        copy_buf(state_key, id, 32);
        uint8_t state_data[104]; // refund_to (34) + pay_to (34) + id (32) + padding
        copy_buf(state_data, refund_to, 34);
        copy_buf(state_data + 34, pay_to, 34);
        copy_buf(state_data + 68, id, 32);
        hook_state_set(state_data, 104, state_key, STATE_KEY_SIZE);
        return 0;
    }

    // Handle EscrowCancel (renter cancel)
    if (BUF_COMPARE(tx_type, "EscrowCancel", 12) == 0)
    {
        // Get memo
        uint8_t memo[MEMO_SIZE], id[33];
        uint32_t memo_len = MEMO_SIZE;
        if (hook_tx_memo(memo, &memo_len) != OK)
            return -7;
        uint32_t id_len = 32;
        if (parse_memo_field(memo, memo_len, "id", id, &id_len) != 0)
            return -8;

        // Validate sender and state
        uint8_t state_key[STATE_KEY_SIZE], state_data[104], refund_to[35], pay_to[35];
        copy_buf(state_key, id, 32);
        if (hook_state_get(state_data, 104, state_key, STATE_KEY_SIZE) != 104)
            return -9;
        copy_buf(refund_to, state_data, 34);
        copy_buf(pay_to, state_data + 34, 34);
        uint8_t sender[35];
        if (hook_tx_account(sender, 35) != OK || BUF_COMPARE(sender, refund_to, 34) != 0)
            return -10; // Invalid sender

        // Get escrow amounts
        uint8_t escrow_id[32];
        if (hook_tx_escrow_id(escrow_id, 32) != OK)
            return -11;
        uint8_t escrow_data[256];
        uint32_t escrow_len = 256;
        if (ledger_entry(escrow_data, &escrow_len, escrow_id, 32) != OK)
            return -12;
        uint64_t xrp_amount = INT64_FROM_BUF(escrow_data + 20);   // XRP drops
        uint64_t rlusd_amount = INT64_FROM_BUF(escrow_data + 28); // RLUSD drops

        // Get current DEX rate
        uint64_t rlusd_per_xrp = get_dex_rate(RLUSD_CURRENCY, RLUSD_ISSUER);
        if (rlusd_per_xrp == -1)
            return -13;
        uint64_t xrp_value = (xrp_amount * rlusd_per_xrp) / 1000000; // XRP in RLUSD
        uint64_t total_value = xrp_value + rlusd_amount;
        uint64_t original_value = rlusd_amount + (xrp_amount * INT64_FROM_BUF(state_data + 68)) / 1000000; // Initial RLUSD value

        // Refund lesser value
        uint8_t refund_tx[256];
        uint32_t refund_len = 256;
        if (xrp_value < original_value)
        {
            // Refund XRP
            if (prepare_payment(refund_tx, &refund_len, refund_to, xrp_amount, XRP_CURRENCY, XRP_ISSUER) != OK)
                return -14;
            emit_tx(refund_tx, refund_len);
            // Send RLUSD spread to system
            uint64_t spread = total_value - original_value;
            if (prepare_payment(refund_tx, &refund_len, SYSTEM_WALLET, spread, RLUSD_CURRENCY, RLUSD_ISSUER) != OK)
                return -15;
            emit_tx(refund_tx, refund_len);
        }
        else
        {
            // Refund RLUSD
            uint64_t refund_amount = original_value;
            if (xrp_value > original_value)
            {
                // Convert XRP to RLUSD for spread
                uint8_t amm_tx[256];
                uint32_t amm_len = 256;
                if (prepare_amm_trade(amm_tx, &amm_len, xrp_amount, RLUSD_CURRENCY, RLUSD_ISSUER, xrp_amount * rlusd_per_xrp / 1000000, MAX_SLIPPAGE) != OK)
                {
                    return -16;
                }
                emit_tx(amm_tx, amm_len);
                refund_amount += xrp_amount * rlusd_per_xrp / 1000000;
            }
            if (prepare_payment(refund_tx, &refund_len, refund_to, refund_amount, RLUSD_CURRENCY, RLUSD_ISSUER) != OK)
                return -17;
            emit_tx(refund_tx, refund_len);
        }

        // Clear state
        hook_state_set(0, 0, state_key, STATE_KEY_SIZE);
        return 0;
    }

    // Handle EscrowFinish (EO pickup)
    if (BUF_COMPARE(tx_type, "EscrowFinish", 11) == 0)
    {
        // Get memo
        uint8_t memo[MEMO_SIZE], id[33];
        uint32_t memo_len = MEMO_SIZE;
        if (hook_tx_memo(memo, &memo_len) != OK)
            return -18;
        uint32_t id_len = 32;
        if (parse_memo_field(memo, memo_len, "id", id, &id_len) != 0)
            return -19;

        // Validate sender and state
        uint8_t state_key[STATE_KEY_SIZE], state_data[104], refund_to[35], pay_to[35];
        copy_buf(state_key, id, 32);
        if (hook_state_get(state_data, 104, state_key, STATE_KEY_SIZE) != 104)
            return -20;
        copy_buf(refund_to, state_data, 34);
        copy_buf(pay_to, state_data + 34, 34);
        uint8_t sender[35];
        if (hook_tx_account(sender, 35) != OK || BUF_COMPARE(sender, pay_to, 34) != 0)
            return -21; // Invalid sender

        // Get escrow amounts
        uint8_t escrow_id[32];
        if (hook_tx_escrow_id(escrow_id, 32) != OK)
            return -22;
        uint8_t escrow_data[256];
        uint32_t escrow_len = 256;
        if (ledger_entry(escrow_data, &escrow_len, escrow_id, 32) != OK)
            return -23;
        uint64_t xrp_amount = INT64_FROM_BUF(escrow_data + 20);   // XRP drops
        uint64_t rlusd_amount = INT64_FROM_BUF(escrow_data + 28); // RLUSD drops

        // Get current DEX rate
        uint64_t rlusd_per_xrp = get_dex_rate(RLUSD_CURRENCY, RLUSD_ISSUER);
        if (rlusd_per_xrp == -1)
            return -24;
        uint64_t xrp_value = (xrp_amount * rlusd_per_xrp) / 1000000; // XRP in RLUSD
        uint64_t total_value = xrp_value + rlusd_amount;
        uint64_t original_value = rlusd_amount + (xrp_amount * INT64_FROM_BUF(state_data + 68)) / 1000000; // Initial RLUSD value

        // Pay lesser value
        uint8_t payment_tx[256];
        uint32_t payment_len = 256;
        if (xrp_value < original_value)
        {
            // Pay XRP
            if (prepare_payment(payment_tx, &payment_len, pay_to, xrp_amount, XRP_CURRENCY, XRP_ISSUER) != OK)
                return -25;
            emit_tx(payment_tx, payment_len);
            // Send RLUSD spread to system
            uint64_t spread = total_value - original_value;
            if (prepare_payment(payment_tx, &payment_len, SYSTEM_WALLET, spread, RLUSD_CURRENCY, RLUSD_ISSUER) != OK)
                return -26;
            emit_tx(payment_tx, payment_len);
        }
        else
        {
            // Pay RLUSD
            uint64_t pay_amount = original_value;
            if (xrp_value > original_value)
            {
                // Convert XRP to RLUSD for spread
                uint8_t amm_tx[256];
                uint32_t amm_len = 256;
                if (prepare_amm_trade(amm_tx, &amm_len, xrp_amount, RLUSD_CURRENCY, RLUSD_ISSUER, xrp_amount * rlusd_per_xrp / 1000000, MAX_SLIPPAGE) != OK)
                {
                    return -27;
                }
                emit_tx(amm_tx, amm_len);
                pay_amount += xrp_amount * rlusd_per_xrp / 1000000;
            }
            if (prepare_payment(payment_tx, &payment_len, pay_to, pay_amount, RLUSD_CURRENCY, RLUSD_ISSUER) != OK)
                return -28;
            emit_tx(payment_tx, payment_len);
        }

        // Clear state
        hook_state_set(0, 0, state_key, STATE_KEY_SIZE);
        return 0;
    }

    return 0;
}