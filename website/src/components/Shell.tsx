import type { ReactNode } from 'react';
import { cn } from '@/lib/utils';

interface ShellProps {
  children: ReactNode;
  className?: string;
  as?: keyof JSX.IntrinsicElements;
  id?: string;
}

/** The shared centred container: one width, one side padding. */
export default function Shell({ children, className, as: Tag = 'div', id }: ShellProps) {
  return (
    <Tag id={id} className={cn('mx-auto w-full max-w-shell px-gutter', className)}>
      {children}
    </Tag>
  );
}
