import { Link } from "@tanstack/react-router";
import { Menu, X } from "lucide-react";
import { useState, type ReactNode } from "react";
import "../../warlock-navigation.css";

const navItems = [
  { to: "/products" as const, label: "PRODUCTS" },
  { to: "/generate" as const, label: "GENERATE" },
  { to: "/presets" as const, label: "PRESETS" },
  { to: "/skins" as const, label: "SKINS" },
  { to: "/pricing" as const, label: "PRICING" },
  { to: "/support" as const, label: "SUPPORT" },
];

type SiteHeaderProps = {
  section?: string;
  active?: (typeof navItems)[number]["to"];
  rightSlot?: ReactNode;
};

export function SiteHeader({ section = "PLUGINS", active, rightSlot }: SiteHeaderProps) {
  const [menuOpen, setMenuOpen] = useState(false);

  return (
    <header className="warlock-site-header">
      <Link to="/" className="warlock-site-brand" aria-label="WARLOCK home" onClick={() => setMenuOpen(false)}>
        <img src="/brand/warlock-core.svg" alt="" />
        <span><strong>WARLOCK</strong><small>{section}</small></span>
      </Link>
      <nav className="warlock-site-desktop-nav" aria-label="Primary navigation">
        {navItems.map((item) => <Link key={item.to} to={item.to} className={active === item.to ? "is-active" : ""}>{item.label}</Link>)}
      </nav>
      <div className="warlock-site-actions">
        {rightSlot ?? <Link to="/generate" search={{ prompt: "" }} className="warlock-site-create">CREATE</Link>}
        <button className="warlock-site-menu-trigger" type="button" aria-expanded={menuOpen} aria-label={menuOpen ? "Close menu" : "Open menu"} onClick={() => setMenuOpen((value) => !value)}>
          {menuOpen ? <X size={20} /> : <Menu size={20} />}
        </button>
      </div>
      {menuOpen ? <nav className="warlock-site-mobile-menu" aria-label="Mobile navigation">
        {navItems.map((item, index) => <Link key={item.to} to={item.to} onClick={() => setMenuOpen(false)}><span>{String(index + 1).padStart(2, "0")}</span><strong>{item.label}</strong></Link>)}
        <Link to="/generate" search={{ prompt: "" }} className="warlock-site-mobile-create" onClick={() => setMenuOpen(false)}>CREATE WITH WARLOCK</Link>
      </nav> : null}
    </header>
  );
}
