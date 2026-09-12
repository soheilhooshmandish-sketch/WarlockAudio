# WARLOCK Website Deployment Gate

Production deployment is never inferred from committed build output.

## Canonical deployment path

WARLOCK deploys through the existing **Cloudflare native Git integration** connected to the GitHub repository.

Canonical Cloudflare project/site:
- Cloudflare project: `warlock-plugins-com`
- Public domain: `warlock-plugins.com`
- Source repository: `soheilhooshmandish-sketch/WarlockAudio`

Do not create a second GitHub Actions -> API token -> Cloudflare deployment pipeline unless the native Git integration is intentionally retired in a documented migration.

## Build compatibility proof

Website CI still builds Nitro with `cloudflare_pages` and boots the generated output through Wrangler/Workerd locally. That proves the application is compatible with Cloudflare runtime before Git-connected deployment.

This CI proof is not itself deployment proof.

## Preview first

Use the existing Cloudflare Git integration for preview/non-production deployments. The dedicated Git branch `preview/cloudflare` may be used to trigger a preview when Cloudflare project settings allow branch previews.

Required preview proof:
1. Cloudflare reports a preview deployment for the intended Git commit;
2. preview URL exists;
3. deployed commit SHA matches the reviewed source;
4. Factory public intake remains disarmed by default;
5. live billing remains off;
6. all primary routes render successfully;
7. account/login behavior is checked with preview auth configuration;
8. no browser bundle contains Factory secrets;
9. mobile/desktop/keyboard/reduced-motion checks pass;
10. Founder reviews the preview before any production-domain change.

No GitHub repository secret is required merely to use the existing Cloudflare Git integration.

## Production

`warlock-plugins.com` remains the production domain. Do not change production branch/domain settings or promote an experimental preview until the launch blockers in `WEBSITE_NEXT.md` are resolved and Founder approval is explicit.

Committed `.vercel/output` is legacy generated build material, not deployment proof. `.vercel/` is ignored going forward.
