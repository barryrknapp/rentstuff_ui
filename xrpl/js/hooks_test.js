async function testHooks() {
    const spreadHookAddress = await setupSpreadHookAccount();
    const rentalHookAddress = await setupHookAccount(spreadHookAddress);
    const client = new Client('wss://s.devnet.rippletest.net:51234');
    await client.connect();

    // Test payment
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

    // Test EscrowCancel
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

    // Test EscrowFinish
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
testHooks().catch(console.error);