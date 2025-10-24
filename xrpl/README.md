Deployment StepsCompile Hooks:Save as hook_xrpl_escrow_spread.c and hook_fse_burn.c.
Compile: wasm32-unknown-emscripten (RippleX tools).
Output: hook_xrpl_escrow_spread.wasm, hook_fse_burn.wasm.
Store in project folder (e.g., hooks/).

Run JS:Execute setup_hook_fse_burn.js to get spreadHookAddress.
Pass to setup_hook_xrpl_escrow.js (update SYSTEM_WALLET).
Run testHooks on XRPL devnet.

Keyless Setup:Generate accounts with temporary keys.
Run TrustSet (RLUSD for Rental Hook; RLUSD/FSE for Spread Hook).
Deploy via SetHook (reference .wasm files).
Disable master keys (AccountSet, sfDisableMaster).
Verify on Xaman/explorer.

Frontend (dApp):Use xrpl.js, Xaman for signing.
Generate { "refund_to", "pay_to", "id" } at payment, { "id" } at cancel/pickup.
Store rental data in DB.

Platform Wallet:Set up rPlatformWallet... with RLUSD trustline (if receiving RLUSD).
Control its key for system costs.

Legal:File WY DUNA ($200-500) for both Hooks.
Confirm with WY lawyer ($1-2k) for MSB/MTL exemption.

