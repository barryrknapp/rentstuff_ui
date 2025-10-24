const { Client, Wallet } = require('xrpl');

async function setupSpreadHookAccount() {
    const client = new Client('wss://s.devnet.rippletest.net:51234'); // Devnet
    await client.connect();

    // Generate Spread Hook account
    const spreadHookWallet = Wallet.generate();
    console.log('Spread Hook Account:', spreadHookWallet.classicAddress);

    // Fund Spread Hook account
    await client.fundWallet(spreadHookWallet);

    // Set trustline for RLUSD
    const rlusdTrustSetTx = {
        TransactionType: 'TrustSet',
        Account: spreadHookWallet.classicAddress,
        LimitAmount: {
            currency: 'RLUSD',
            issuer: 'rRLUSD...', // Replace
            value: '1000000'
        }
    };
    await client.submitAndWait(rlusdTrustSetTx, { wallet: spreadHookWallet });

    // Set trustline for FSE
    const fseTrustSetTx = {
        TransactionType: 'TrustSet',
        Account: spreadHookWallet.classicAddress,
        LimitAmount: {
            currency: 'FSE',
            issuer: 'rFSEIssuer...', // Replace
            value: '1000000'
        }
    };
    await client.submitAndWait(fseTrustSetTx, { wallet: spreadHookWallet });

    // Deploy Spread Hook
    const setHookTx = {
        TransactionType: 'SetHook',
        Account: spreadHookWallet.classicAddress,
        Hooks: [{ Hook: { CreateCode: '...' /* WASM binary from Spread Hook C */ } }]
    };
    await client.submitAndWait(setHookTx, { wallet: spreadHookWallet });

    // Disable master key
    const accountSetTx = {
        TransactionType: 'AccountSet',
        Account: spreadHookWallet.classicAddress,
        SetFlag: 1 // sfDisableMaster
    };
    await client.submitAndWait(accountSetTx, { wallet: spreadHookWallet });

    await client.disconnect();
    return spreadHookWallet.classicAddress;
}

async function testHooks(rentalHookAddress, spreadHookAddress) {
    const client = new Client('wss://s.devnet.rippletest.net:51234');
    await client.connect();

    // Test Rental Hook payment
    const renterWallet = Wallet.generate();
    await client.fundWallet(renterWallet);
    const paymentTx = {
        TransactionType: 'Payment',
        Account: renterWallet.classicAddress,
        Destination: rentalHookAddress,
        Amount: '2000000', // 2 XRP
        Memos: [{
            Memo: {
                MemoData: Buffer.from(JSON.stringify({
                    refund_to: renterWallet.classicAddress,
                    pay_to: 'rEOAddress...', // Replace
                    id: 'external_db_key_123'
                })).toString('hex')
            }
        }]
    };
    const paymentResult = await client.submitAndWait(paymentTx, { wallet: renterWallet });
    console.log('Payment:', paymentResult);

    // Test EscrowCancel (sends spread to Spread Hook)
    const escrowSequence = paymentResult.result.Sequence; // Adjust
    const cancelTx = {
        TransactionType: 'EscrowCancel',
        Account: renterWallet.classicAddress,
        Owner: rentalHookAddress,
        OfferSequence: escrowSequence,
        Memos: [{ Memo: { MemoData: Buffer.from(JSON.stringify({ id: 'external_db_key_123' })).toString('hex') } }]
    };
    const cancelResult = await client.submitAndWait(cancelTx, { wallet: renterWallet });
    console.log('Cancel:', cancelResult);

    // Test EscrowFinish (sends spread to Spread Hook)
    const eoWallet = Wallet.generate();
    await client.fundWallet(eoWallet);
    const finishTx = {
        TransactionType: 'EscrowFinish',
        Account: eoWallet.classicAddress,
        Owner: rentalHookAddress,
        OfferSequence: escrowSequence,
        Memos: [{ Memo: { MemoData: Buffer.from(JSON.stringify({ id: 'external_db_key_123' })).toString('hex') } }]
    };
    const finishResult = await client.submitAndWait(finishTx, { wallet: eoWallet });
    console.log('Pickup:', finishResult);

    await client.disconnect();
}

// Run setup and test
async function main() {
    const rentalHookAddress = await setupHookAccount(); // From prior JS code
    const spreadHookAddress = await setupSpreadHookAccount();
    await testHooks(rentalHookAddress, spreadHookAddress);
}
main().catch(console.error);