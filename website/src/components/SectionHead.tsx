import type { ReactNode } from 'react';

interface SectionHeadProps {
  number: string;
  title: string;
  aside?: ReactNode;
}

/** Opens a numbered section: accent number, italic title, line rule. */
export default function SectionHead({ number, title, aside }: SectionHeadProps) {
  return (
    <div className="flex flex-wrap items-baseline justify-between gap-x-6 gap-y-3 pb-5 border-b border-line">
      <div className="flex items-baseline gap-4">
        <span className="lbl text-accent">{number}</span>
        <h2 className="fx-display-i text-display-md text-ink">{title}</h2>
      </div>
      {aside}
    </div>
  );
}
