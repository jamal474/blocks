import Shell from './Shell';
import SectionHead from './SectionHead';
import { internals, stack } from '@/data/content';

export default function UnderTheHood() {
  return (
    <Shell as="section" id="hood" className="pt-section">
      <SectionHead number="02" title="Under the hood" />

      <ol className="m-0 list-none p-0">
        {internals.map((row) => (
          <li key={row.title} className="row-nudge flex flex-wrap gap-x-10 gap-y-2 border-b border-line-soft py-[26px]">
            <h3 className="fx-title-i m-0 flex-[1_1_260px] text-2xl leading-[1.2]">{row.title}</h3>
            <p className="fx-body m-0 flex-[999_1_420px] text-ink-muted">{row.body}</p>
          </li>
        ))}
      </ol>

      <ul className="m-0 mt-6 flex list-none flex-wrap gap-2 p-0">
        {stack.map((item) => (
          <li key={item} className="lbl border border-line px-3 py-2 text-ink-soft">
            {item}
          </li>
        ))}
      </ul>
    </Shell>
  );
}
