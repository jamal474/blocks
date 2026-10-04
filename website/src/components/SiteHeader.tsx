import { Github } from 'lucide-react';
import Mark from './Mark';
import { cn } from '@/lib/utils';
import { REPO_URL } from '@/data/site';
import { NAV } from '@/data/content';
import { useActiveSection } from '@/lib/useActiveSection';

const NAV_IDS = NAV.map((n) => n.id);

/** The shared masthead: mark and wordmark, numbered nav, GitHub, Download. */
export default function SiteHeader() {
  const active = useActiveSection(NAV_IDS);

  return (
    <header className="sticky top-0 z-40 border-b border-line-soft bg-paper/90 backdrop-blur-md">
      <div className="mx-auto flex w-full max-w-shell flex-wrap items-center justify-between gap-x-8 gap-y-3 px-gutter py-4">
        <a href="#top" className="flex min-h-[44px] items-center gap-3">
          <Mark />
          <span className="fx-title-i text-[26px] leading-none">City Bloxx</span>
        </a>

        <nav aria-label="Sections" className="hidden md:block">
          <ol className="flex flex-wrap items-baseline gap-x-7">
            {NAV.map((item) => {
              const current = active === item.id;
              return (
                <li key={item.id}>
                  <a
                    href={`#${item.id}`}
                    aria-current={current ? 'true' : undefined}
                    className={cn(
                      'flex items-baseline gap-1.5 py-3 italic transition-colors hover:text-accent',
                      current ? 'text-accent' : 'text-ink'
                    )}
                  >
                    <span className={cn('lbl not-italic', current ? 'text-accent' : 'text-ink-dim')}>{item.num}</span>
                    {item.label}
                  </a>
                </li>
              );
            })}
          </ol>
        </nav>

        <div className="flex items-center gap-2">
          <a
            href={REPO_URL}
            target="_blank"
            rel="noreferrer"
            aria-label="City Bloxx on GitHub"
            className="inline-flex size-11 items-center justify-center text-ink-muted transition-colors hover:text-accent"
          >
            <Github size={18} strokeWidth={1.6} />
          </a>
          <a href="#download" className="btn lbl">
            Download
          </a>
        </div>
      </div>
    </header>
  );
}
