import { createFileRoute } from "@tanstack/react-router";
import { AccountShell } from "@/components/account/account-shell";

export const Route = createFileRoute("/account_/licenses")({
  component: () => <AccountShell section="licenses" />,
});
