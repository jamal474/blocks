const FLOORS = [
  { shift: 14, accent: false },
  { shift: -9, accent: false },
  { shift: 6, accent: true },
  { shift: -3, accent: false },
  { shift: 0, accent: false },
];

/**
 * Fig. 1: the crane swings and the tower sways. The page's one illustrative
 * animation; it stops under prefers-reduced-motion.
 */
export default function CraneFigure() {
  return (
    <figure className="mt-[clamp(48px,7vw,80px)]">
      <div
        className="grid-paper relative h-[440px] overflow-hidden border border-line"
        role="img"
        aria-label="A crane swinging a block above a five-storey tower that sways slightly"
      >
        {/* Crane arm and pivot */}
        <div className="absolute inset-x-0 top-10 h-0.5 bg-ink" />
        <div className="absolute left-1/2 top-[34px] -ml-[7px] size-3.5 rounded-full border-2 border-ink bg-paper-soft" />

        {/* Rope and hanging block */}
        <div className="absolute left-1/2 top-[41px] -ml-[60px] h-[170px] w-[120px] origin-top animate-swing">
          <div className="absolute left-[59px] top-0 h-32 w-0.5 bg-ink" />
          <div className="absolute left-0 top-32 h-10 w-[120px] border-2 border-ink bg-signal" />
        </div>
        <span className="lbl absolute left-[calc(50%+18px)] top-[60px] text-ink-muted">θ</span>

        {/* Tower */}
        <div className="absolute bottom-12 left-1/2 -ml-[70px] flex w-[140px] origin-bottom animate-sway flex-col items-center">
          {FLOORS.map((f, i) => (
            <div
              key={i}
              className={`h-10 w-[120px] border-2 border-ink ${i > 0 ? 'border-t-0' : ''} ${f.accent ? 'bg-accent' : 'bg-paper-soft'}`}
              style={{ transform: `translateX(${f.shift}px)` }}
            />
          ))}
        </div>

        <div className="absolute inset-x-0 bottom-[46px] h-0.5 bg-ink" />
        <span className="lbl absolute bottom-4 left-5 text-ink-muted">Ground</span>
        <span className="lbl absolute right-5 top-4 text-ink-muted">Crane arm</span>
      </div>
      <figcaption className="mt-3 font-mono text-[11px] tracking-[0.08em] text-ink-dim">
        Fig. 1 — The crane swings, the tower answers.
      </figcaption>
    </figure>
  );
}
