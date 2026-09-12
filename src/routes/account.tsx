import { createFileRoute } from "@tanstack/react-router";
import { AccountShell } from "@/components/account/account-shell";

export const Route = createFileRoute("/account")({
  component: () => <AccountShell section="overview" />,
});
