"use client";

import { useEffect, useState } from "react";

export default function CountDown({
  scheduledTime = 0,
}: {
  scheduledTime?: number;
}) {
  const [time, setTime] = useState(
    Math.round((scheduledTime - Date.now()) / 1000),
  );

  useEffect(() => {
    if (time < 0) return;

    const interval = setInterval(() => {
      const newTime = Math.round((scheduledTime - Date.now()) / 1000);
      setTime(newTime);
      if (newTime < 0) clearInterval(interval);
    }, 1000);

    return () => clearInterval(interval);
  }, []);

  if (time < 0) return <>{`(Pending)`}</>;

  return <>{`(In ${time}s)`}</>;
}
