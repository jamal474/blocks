/** The City Bloxx mark: three stacked, slightly offset blocks. */
export default function Mark() {
  return (
    <span aria-hidden="true" className="flex w-7 flex-col items-center gap-0.5">
      <span className="h-[7px] w-3.5 translate-x-[3px] border border-ink bg-signal" />
      <span className="h-[7px] w-[18px] border border-ink bg-paper-soft" />
      <span className="h-[7px] w-[18px] -translate-x-0.5 border border-ink bg-paper-soft" />
    </span>
  );
}
