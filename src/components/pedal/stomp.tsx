import { cn } from "@/lib/utils";

type StompProps = {
  engaged: boolean;
  awake: boolean;
  onToggle: () => void;
};

export function StompSwitch({ engaged, awake, onToggle }: StompProps) {
  return (
    <div className="flex flex-col items-center gap-2">
      <div
        className={cn(
          "led-lamp size-3 rounded-full transition-[box-shadow,background-color,opacity] duration-(--motion-fast)",
          engaged && awake ? "led-on" : "led-off",
        )}
        aria-hidden
      />
      <div className="stomp-well">
        <button
          type="button"
          aria-pressed={engaged && awake}
          aria-label={engaged && awake ? "Bypass pedal" : "Engage pedal"}
          onClick={onToggle}
          className={cn(
            "stomp-cap relative size-16 rounded-full sm:size-[4.75rem]",
            "focus-visible:outline-none focus-visible:ring-2 focus-visible:ring-frost/60",
            "active:translate-y-0.5",
          )}
        >
          <span className="sr-only">{engaged && awake ? "On" : "Off"}</span>
        </button>
      </div>
    </div>
  );
}
