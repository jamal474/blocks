import { useMemo } from 'react';
import SiteHeader from './components/SiteHeader';
import Hero from './components/Hero';
import HowItPlays from './components/HowItPlays';
import UnderTheHood from './components/UnderTheHood';
import Download from './components/Download';
import SiteFooter from './components/SiteFooter';
import { detectPlatform, useRelease } from './lib/useRelease';

export default function App() {
  const release = useRelease();
  const detected = useMemo(detectPlatform, []);

  return (
    <>
      <SiteHeader />
      <main>
        <Hero release={release} detected={detected} />
        <HowItPlays />
        <UnderTheHood />
        <Download release={release} detected={detected} />
      </main>
      <SiteFooter />
    </>
  );
}
