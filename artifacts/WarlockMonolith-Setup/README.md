# WARLOCK MONOLITH — GitHub demo / lifetime setup

Warlock Audio · www.warlock-plugins.com  
Product: MONOLITH · Model 1666  
Distribution: GitHub Releases on `soheilhooshmandish-sketch/WarlockAudio`

This folder is the published edition pipeline. It does **not** use a store. Customers download one installer from GitHub. You issue signed codes from the factory License Desk.

## What the customer receives

| Edition | How they get it | What the installer writes |
|---|---|---|
| Evaluation | Same setup.exe, choose “14-day evaluation” | `%ProgramData%\Warlock Audio\MONOLITH\license.key` as a DEMO JSON clock |
| Lifetime | Same setup.exe, paste a `WL1.MONOLITH.LIFE...` code | The signed key into the same file |

There is **one** setup binary. There is **not** a lifetime generator inside that binary. If the generator shipped in the customer setup, every download would mint free lifetime seats.

## What you run (factory)

```
cd tools/license-desk
node warlock-license.mjs init-keys
node warlock-license.mjs issue --edition lifetime --email buyer@studio.tld
node warlock-license.mjs issue --edition demo --email trial@studio.tld --days 14
node warlock-license.mjs verify "WL1.MONOLITH.LIFE...."
npm run desk
```

`init-keys` writes `private.pem` and `public.pem`. `private.pem` stays on this machine and in the GitHub secret `WARLOCK_LICENSE_PRIVATE_KEY`. It is gitignored.

Open http://127.0.0.1:1666 for the local desk. It refuses non-localhost clients.

## GitHub secrets

Repository: `soheilhooshmandish-sketch/WarlockAudio`

| Secret | Contents |
|---|---|
| `WARLOCK_LICENSE_PRIVATE_KEY` | full PKCS#8 `private.pem` text |
| `WARLOCK_LICENSE_PUBLIC_KEY` | full SPKI `public.pem` text |

After `init-keys`, paste those two files into Settings → Secrets and variables → Actions.

Also embed `public.pem` in `plugin/WarlockLicense.h` before the 1.0 plugin build.

## GitHub Releases

1. Build the VST3 (existing Windows workflow for the DSP project).
2. Copy the bundle to `artifacts/WarlockMonolith-Setup/payload/WarlockMonolith.vst3/`.
3. Run workflow `MONOLITH demo + lifetime setup`.
4. Attach `Warlock_MONOLITH_Setup_x64_1.0.0.exe` to the GitHub Release.
5. After a sale, run the desk, paste the Key line into the receipt.

Do not attach `private.pem`, `issued-ledger.jsonl`, or the License Desk with a private key.

## Seat policy

Lifetime: two machines, perpetual.  
Evaluation: one machine, fourteen days from install.
