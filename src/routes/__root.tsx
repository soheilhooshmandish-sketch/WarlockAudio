import { createRootRoute, HeadContent, Outlet, Scripts } from "@tanstack/react-router";
import { useEffect } from "react";
import { AuthProvider } from "@/lib/auth/provider";
import { PreviewHostBridge } from "@/components/preview-host-bridge";
import appCss from "../styles.css?url";
import warlockHomeCss from "../warlock-home.css?url";
import warlockPagesCss from "../warlock-pages.css?url";
import warlockAtmosphereCss from "../warlock-atmosphere.css?url";
import warlockProductsCss from "../warlock-products.css?url";

const APP_NAME = "WARLOCK PLUGINS — Sound Beyond Reality";

type TimeAtmosphere = "morning" | "day" | "dusk" | "night";

function getTimeAtmosphere(): TimeAtmosphere {
  const hour = new Date().getHours();
  if (hour >= 6 && hour < 10) return "morning";
  if (hour >= 10 && hour < 17) return "day";
  if (hour >= 17 && hour < 21) return "dusk";
  return "night";
}

function RootDocument() {
  useEffect(() => {
    const applyAtmosphere = () => {
      document.documentElement.dataset.warlockTime = getTimeAtmosphere();
    };

    applyAtmosphere();
    const interval = window.setInterval(applyAtmosphere, 60_000);
    return () => window.clearInterval(interval);
  }, []);

  return (
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
  );
}

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
      { rel: "stylesheet", href: warlockPagesCss },
      { rel: "stylesheet", href: warlockAtmosphereCss },
      { rel: "stylesheet", href: warlockProductsCss },
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
  component: RootDocument,
});
