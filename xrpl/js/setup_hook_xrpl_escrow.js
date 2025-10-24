const { Client, Wallet } = require('xrpl');
const fs = require('fs');

async function setupHookAccount(spreadHookAddress) {
    const client = new Client('wss://s.devnet.rippletest.net:51234');
    await client.connect();

    // Generate Rental Hook account
    const hookWallet = Wallet.generate();
    console.log('Rental Hook Account:', hookWallet.classicAddress);

    // Fund account
    await client.fundWallet(hookWallet);

    // Set RLUSD trustline
    const trustSetTx = {
        TransactionType: 'TrustSet',
        Account: hookWallet.classicAddress,
        LimitAmount: {
            currency: 'RLUSD',
            issuer: 'rRLUSD...', // Replace
            value: '1000000'
        }
    };
    await client.submitAndWait(trustSetTx, { wallet: hookWallet });

    // Deploy Rental Hook
    const setHookTx = {
        TransactionType: 'SetHook',
        Account: hookWallet.classicAddress,
        Hooks: [{ Hook: { CreateCode: fs.readFileSync('hook_xrpl_escrow_spread.wasm').toString('hex') } }]
    };
    await client.submitAndWait(setHookTx, { wallet: hookWallet });

    // Disable master key
    const accountSetTx = {
        TransactionType: 'AccountSet',
        Account: hookWallet.classicAddress,
        SetFlag: 1 // sfDisableMaster
    };
    await client.submitAndWait(accountSetTx, { wallet: hookWallet });

    await client.disconnect();
    return hookWallet.classicAddress;
}