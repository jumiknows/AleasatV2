"use client";

import { useRouter } from "next/navigation";
import { useEffect } from "react";

export default function Refresher() {
  const router = useRouter();

  useEffect(() => {
    const interval = setInterval(() => {
      router.refresh();
    }, 5000);
    return () => clearInterval(interval);
  }, []);

  return <></>;
}
