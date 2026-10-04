import Shell from './Shell';
import SectionHead from './SectionHead';
import { controls, steps } from '@/data/content';

export default function HowItPlays() {
  return (
    <Shell as="section" id="play" className="pt-section">
      <SectionHead number="01" title="How it plays" />

      <div className="grid grid-cols-[repeat(auto-fit,minmax(260px,1fr))] gap-x-12">
        {steps.map((step, i) => (
          <div key={step.title} className="border-b border-line-soft py-7">
            <p className="lbl mb-3.5 text-ink-dim">Step {i + 1}</p>
            <h3 className="fx-title-i mb-2 text-[26px]">{step.title}</h3>
            <p className="fx-body text-ink-muted">{step.body}</p>
          </div>
        ))}
      </div>

      <div className="mt-10 border border-line bg-paper-soft p-7">
        <div className="mb-[18px] flex flex-wrap items-baseline justify-between gap-x-6 gap-y-2">
          <p className="lbl">Instability</p>
          <p className="font-mono text-[11px] tracking-[0.08em] text-ink-dim">
            Misses feed it on a curve (offset^1.6); a near-perfect drop calms it
          </p>
        </div>
        <div
          className="relative h-3 border border-line bg-paper-deep"
          role="img"
          aria-label="Instability meter from 0 (steady) to 1.0 (collapse)"
        >
          <div className="absolute inset-y-0 left-0 w-[64%] bg-accent" />
          <div className="absolute -inset-y-1.5 left-[64%] w-0.5 bg-ink" />
        </div>
        <div className="mt-2.5 flex justify-between">
          <span className="lbl text-ink-dim">0.0 · steady</span>
          <span className="lbl text-ink-dim">0.5</span>
          <span className="lbl text-ink">1.0 · collapse</span>
        </div>
      </div>

      <div className="mt-6 flex flex-wrap items-center gap-x-3.5 gap-y-2.5">
        <span className="lbl text-ink-dim">Controls</span>
        {controls.map(([key, action]) => (
          <span key={key} className="mr-2.5 inline-flex items-center gap-2.5">
            <kbd className="kbd">{key}</kbd>
            <span className="text-[15px] text-ink-muted">{action}</span>
          </span>
        ))}
      </div>
    </Shell>
  );
}
