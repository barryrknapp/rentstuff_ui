#include "hookapi.h"

// Constants
#define PLATFORM_WALLET "rPlatformWallet..." // Replace: Your controlled wallet for costs
#define BURN_ADDRESS "rBurnAddress..."       // Replace: Null address for FSE burn
#define FSE_ISSUER "rFSEIssuer..."           // Replace: Actual FSE issuer
#define RLUSD_ISSUER "rRLUSD..."             // Replace: Actual RLUSD issuer
#define STATE_KEY_SIZE 32
#define MEMO_SIZE 256
#define MIN_AMOUNT 0.000001 // Min drops
#define MAX_SLIPPAGE 200    // 2% slippage

// Debug log
#define DEBUG_LOG(msg, val)                           \
    do                                                \
    {                                                 \
        uint8_t log[64];                              \
        uint32_t log_len = 64;                        \
        uint64_to_string(log, &log_len, val);         \
        trace(msg, sizeof(msg) - 1, log, log_len, 0); \
    } while (0)

// Parse memo: { "id": "123" }
int64_t parse_memo(uint8_t *memo, uint32_t memo_len, uint8_t *id)
{
    if (memo_len < 20 || memo_len > MEMO_SIZE)
        return -1;
    uint8_t *ptr = memo;
    uint32_t i = 0;
    while (i < memo_len && ptr[i++] != ':')
    {
    }
    if (i >= memo_len || ptr[i++] != '"')
        return -2;
    for (int j = 0; j < 32 && i < memo_len; j++)
        id[j] = ptr[i++];
    id[32] = 0;
    return 0;
}

// Get FSE rate (FSE/RLUSD or FSE/XRP)
int64_t get_fse_rate(uint8_t *currency, uint8_t *issuer)
{
    uint8_t amm_info[256];
    uint32_t amm_len = 256;
    if (ledger_amm_info(amm_info, &amm_len, FSE_CURRENCY, FSE_ISSUER, currency, issuer) != OK)
    {
        DEBUG_LOG("FSE AMM fetch failed", 0);
        return -1;
    }
    uint64_t fse_pool = INT64_FROM_BUF(amm_info + 20);
    uint64_t other_pool = INT64_FROM_BUF(amm_info + 28);
    if (other_pool == 0)
    {
        DEBUG_LOG("Zero pool", 0);
        return -1;
    }
    return (fse_pool * 1000000) / other_pool; // FSE per currency (drops)
}

// Hook entry
int64_t hook(uint32_t reserved)
{
    GUARD(32);

    // Get transaction type
    uint8_t tx_type[16];
    if (hook_tx_type(tx_type, 16) != OK)
    {
        DEBUG_LOG("Invalid tx type", 0);
        return -1;
    }

    // Handle Payment (spread)
    if (BUF_COMPARE(tx_type, "Payment", 7) == 0)
    {
        // Get amount and currency
        uint8_t amount_buf[48];
        uint64_t amount_drops;
        uint8_t currency[20], issuer[20];
        if (hook_tx_amount_extended(amount_buf, 48, &amount_drops, currency, issuer) != OK)
        {
            DEBUG_LOG("Invalid amount", 0);
            return -2;
        }
        if (amount_drops < MIN_AMOUNT * 1000000)
        {
            DEBUG_LOG("Amount too small", amount_drops);
            return -3;
        }

        // Get memo
        uint8_t memo[MEMO_SIZE], id[33];
        uint32_t memo_len = MEMO_SIZE;
        if (hook_tx_memo(memo, &memo_len) != OK || parse_memo(memo, memo_len, id) != 0)
        {
            DEBUG_LOG("Invalid memo", memo_len);
            return -4;
        }

        // Store id in state (for tracking)
        uint8_t state_key[STATE_KEY_SIZE];
        copy_buf(state_key, id, 32);
        hook_state_set(id, 32, state_key, STATE_KEY_SIZE);

        // Process RLUSD spread
        if (BUF_COMPARE(currency, RLUSD_CURRENCY, 20) == 0 && BUF_COMPARE(issuer, RLUSD_ISSUER, 20) == 0)
        {
            // Convert 50% to FSE
            uint64_t fse_amount = (amount_drops / 2) / get_fse_rate(RLUSD_CURRENCY, RLUSD_ISSUER);
            if (fse_amount == -1)
            {
                DEBUG_LOG("FSE rate failed", 0);
                return -5;
            }
            uint8_t amm_tx[256];
            uint32_t amm_len = 256;
            if (prepare_amm_trade(amm_tx, &amm_len, amount_drops / 2, FSE_CURRENCY, FSE_ISSUER, fse_amount, MAX_SLIPPAGE) != OK)
            {
                DEBUG_LOG("FSE trade prep failed", 0);
                return -6;
            }
            emit_tx(amm_tx, amm_len);

            // Burn FSE
            uint8_t burn_tx[256];
            uint32_t burn_len = 256;
            if (prepare_payment(burn_tx, &burn_len, BURN_ADDRESS, fse_amount, FSE_CURRENCY, FSE_ISSUER) != OK)
            {
                DEBUG_LOG("FSE burn prep failed", 0);
                return -7;
            }
            emit_tx(burn_tx, burn_len);

            // Send remaining RLUSD to platform
            uint8_t platform_tx[256];
            uint32_t platform_len = 256;
            if (prepare_payment(platform_tx, &platform_len, PLATFORM_WALLET, amount_drops / 2, RLUSD_CURRENCY, RLUSD_ISSUER) != OK)
            {
                DEBUG_LOG("Platform payment prep failed", 0);
                return -8;
            }
            emit_tx(platform_tx, platform_len);
        }
        // Process XRP spread
        else if (BUF_COMPARE(currency, XRP_CURRENCY, 20) == 0)
        {
            // Convert 50% to FSE (via FSE/XRP or FSE/RLUSD)
            uint64_t fse_amount = (amount_drops / 2) / get_fse_rate(XRP_CURRENCY, XRP_ISSUER);
            if (fse_amount == -1)
            {
                DEBUG_LOG("FSE rate failed", 0);
                return -9;
            }
            uint8_t amm_tx[256];
            uint32_t amm_len = 256;
            if (prepare_amm_trade(amm_tx, &amm_len, amount_drops / 2, FSE_CURRENCY, FSE_ISSUER, fse_amount, MAX_SLIPPAGE) != OK)
            {
                DEBUG_LOG("FSE trade prep failed", 0);
                return -10;
            }
            emit_tx(amm_tx, amm_len);

            // Burn FSE
            uint8_t burn_tx[256];
            uint32_t burn_len = 256;
            if (prepare_payment(burn_tx, &burn_len, BURN_ADDRESS, fse_amount, FSE_CURRENCY, FSE_ISSUER) != OK)
            {
                DEBUG_LOG("FSE burn prep failed", 0);
                return -11;
            }
            emit_tx(burn_tx, burn_len);

            // Send remaining XRP to platform
            uint8_t platform_tx[256];
            uint32_t platform_len = 256;
            if (prepare_payment(platform_tx, &platform_len, PLATFORM_WALLET, amount_drops / 2, XRP_CURRENCY, XRP_ISSUER) != OK)
            {
                DEBUG_LOG("Platform payment prep failed", 0);
                return -12;
            }
            emit_tx(platform_tx, platform_len);
        }
        else
        {
            DEBUG_LOG("Invalid currency", 0);
            return -13;
        }

        // Clear state
        hook_state_set(0, 0, state_key, STATE_KEY_SIZE);
        DEBUG_LOG("Spread processed", amount_drops);
        return 0;
    }

    return 0;
}