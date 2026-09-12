import { createRootRoute, HeadContent, Outlet, Scripts } from "@tanstack/react-router";
import { AuthProvider } from "@/lib/auth/provider";
import { PreviewHostBridge } from "@/components/preview-host-bridge";
import appCss from "../styles.css?url";
import warlockHomeCss from "../warlock-home.css?url";

const APP_NAME = "WARLOCK PLUGINS — Sound Beyond Reality";

export const Route = createRootRoute({
  head: () => ({
    meta: [
      { charSet: "utf-8" },
      { name: "viewport", content: "width=device-width, initial-scale=1" },
      { title: APP_NAME },
      {
        name: "description",
        content:
          "WARLOCK PLUGINS — premium audio software, signature effects and a guided audio product generator.",
      },
      { name: "theme-color", content: "#07101A" },
    ],
    links: [
      { rel: "icon", type: "image/svg+xml", href: "/brand/warlock-core.svg" },
      { rel: "stylesheet", href: appCss },
      { rel: "stylesheet", href: warlockHomeCss },
      { rel: "manifest", href: "/__grok/manifest.webmanifest" },
      { rel: "apple-touch-icon", href: "/__grok/icon-180.png" },
      { rel: "preconnect", href: "https://fonts.googleapis.com" },
      { rel: "preconnect", href: "https://fonts.gstatic.com", crossOrigin: "anonymous" },
      {
        rel: "stylesheet",
        href: "https://fonts.googleapis.com/css2?family=Inter:wght@400;500;600&family=Oswald:wght@400;500;600&family=Cinzel:wght@700&family=IM+Fell+English&family=UnifrakturMaguntia&display=swap",
      },
    ],
  }),
  component: () => (
    <html lang="en" className="antialiased" suppressHydrationWarning>
      <head>
        <HeadContent />
      </head>
      <body className="bg-void text-bone">
        <PreviewHostBridge />
        <AuthProvider>
          <Outlet />
        </AuthProvider>
        <Scripts />
      </body>
    </html>
  ),
});
