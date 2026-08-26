import { useEffect, useRef } from "react";
import { getEngine } from "@/lib/audio/pedal-engine";

type LcdProps = {
  title: string;
  subtitle: string;
  engaged: boolean;
};

export function PedalLcd({ title, subtitle, engaged }: LcdProps) {
  const lineRef = useRef<SVGPolylineElement>(null);

  useEffect(() => {
    const line = lineRef.current;
    if (!line) return;
    const wave = new Uint8Array(256);
    const w = 280;
    const h = 56;
    let raf = 0;

    const draw = () => {
      const engine = getEngine();
      const pts: string[] = [];
      if (engine && engaged) {
        engine.analyser.getByteTimeDomainData(wave);
        const step = Math.max(1, Math.floor(wave.length / 70));
        let x = 0;
        for (let i = 0; i < wave.length; i += step) {
          const y = (wave[i] / 255) * h;
          pts.push(`${x.toFixed(1)},${y.toFixed(1)}`);
          x += w / (wave.length / step);
        }
      } else {
        pts.push(`0,${h / 2}`, `${w},${h / 2}`);
      }
      line.setAttribute("points", pts.join(" "));
      raf = requestAnimationFrame(draw);
    };
    raf = requestAnimationFrame(draw);
    return () => cancelAnimationFrame(raf);
  }, [engaged]);

  return (
    <div className="lcd-bezel mx-auto w-full max-w-[15.5rem] overflow-hidden rounded-sm">
      <div className="lcd-screen relative h-14 px-2 py-1.5">
        <svg
          viewBox="0 0 280 56"
          className="absolute inset-0 size-full opacity-60"
          preserveAspectRatio="none"
          aria-hidden
        >
          <polyline
            ref={lineRef}
            fill="none"
            stroke="rgb(231, 244, 246)"
            strokeWidth="1.4"
            points="0,28 280,28"
          />
        </svg>
        <div className="relative z-10 flex h-full flex-col justify-center">
          <p className="font-display text-xs tracking-[0.22em] text-frost-bright uppercase">
            {title}
          </p>
          <p className="font-body text-xs tracking-wide text-frost/80 italic">{subtitle}</p>
        </div>
      </div>
    </div>
  );
}
