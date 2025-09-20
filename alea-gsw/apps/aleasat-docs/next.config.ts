import type { NextConfig } from "next";

const nextConfig: NextConfig = {
  experimental: {},
  eslint: {
    ignoreDuringBuilds: true,
  },
  output: "standalone",
  images: {
    minimumCacheTTL: 0,
    remotePatterns: [
      {
        protocol: "http",
        hostname: "**",
      },
      {
        protocol: "https",
        hostname: "**",
      },
    ],
    formats: ["image/avif", "image/webp"],
  },
  transpilePackages: ["@aleasat/ui"],
};

export default nextConfig;
