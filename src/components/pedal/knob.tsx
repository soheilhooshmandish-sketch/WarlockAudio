import { useRef } from "react";
import { cn } from "@/lib/utils";

type KnobProps = {
  label: string;
  hint?: string;
  readout?: string;
  value: number;
  onChange: (v: number) => void;
  defaultValue?: number;
  compact?: boolean;
  lit?: boolean;
  led?: "blood" | "green";
};

export function Knob({
  label,
  hint,
  readout,
  value,
  onChange,
  defaultValue = 0.5,
  compact = false,
  lit = true,
  led = "blood",
}: KnobProps) {
  const drag = useRef<{ y: number; v: number } | null>(null);
  const angle = -135 + Math.min(1, Math.max(0, value)) * 270;
  const lampOn = lit && value > 0.02;

  const setFromPointer = (clientY: number) => {
    const start = drag.current;
    if (!start) return;
    const next = Math.min(1, Math.max(0, start.v + (start.y - clientY) / 132));
    onChange(next);
  };

  return (
    <div className="flex w-full max-w-24 flex-col items-center gap-1.5">
      <div className="knob-well">
      <button
        type="button"
        aria-label={`${label} ${readout ?? `${Math.round(value * 100)} percent`}`}
        aria-valuemin={0}
        aria-valuemax={100}
        aria-valuenow={Math.round(value * 100)}
        role="slider"
        className={cn(
          "relative focus-visible:outline-none focus-visible:ring-2 focus-visible:ring-frost/60",
          compact ? "size-14" : "size-16 sm:size-[4.5rem]",
        )}
        style={{ touchAction: "none" }}
        onPointerDown={(e) => {
          e.preventDefault();
          e.currentTarget.setPointerCapture(e.pointerId);
          drag.current = { y: e.clientY, v: value };
        }}
        onPointerMove={(e) => {
          if (drag.current) setFromPointer(e.clientY);
        }}
        onPointerUp={() => {
          drag.current = null;
        }}
        onPointerCancel={() => {
          drag.current = null;
        }}
        onDoubleClick={() => onChange(defaultValue)}
        onKeyDown={(e) => {
          const step = e.shiftKey ? 0.1 : 0.04;
          if (e.key === "ArrowUp" || e.key === "ArrowRight") {
            e.preventDefault();
            onChange(Math.min(1, value + step));
          }
          if (e.key === "ArrowDown" || e.key === "ArrowLeft") {
            e.preventDefault();
            onChange(Math.max(0, value - step));
          }
          if (e.key === "Home") onChange(0);
          if (e.key === "End") onChange(1);
        }}
      >
        <span
          className="knob-photo pointer-events-none absolute inset-0"
          style={{ transform: `rotate(${angle}deg)` }}
          aria-hidden
        />
      </button>
      </div>
      <span
        className={cn(
          "led-lamp size-2 rounded-full",
          lampOn ? (led === "green" ? "led-green" : "led-on") : "led-off",
        )}
        aria-hidden
      />
      <span className="flex flex-col items-center gap-0.5">
        {readout ? (
          <span className="knob-readout font-display text-[0.62rem] tracking-wide text-bone">
            {readout}
          </span>
        ) : null}
        <span className="font-display text-[0.62rem] tracking-[0.22em] text-bone-dim uppercase">
          {label}
        </span>
        {hint ? (
          <span className="font-body text-[0.6rem] text-frost/70 italic">{hint}</span>
        ) : null}
      </span>
    </div>
  );
}
