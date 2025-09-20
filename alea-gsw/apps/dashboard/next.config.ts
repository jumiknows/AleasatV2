import type { NextConfig } from "next";
import webpack from "webpack";

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
  async headers() {
    return [
      {
        source: "/model:path*",
        headers: [{ key: "Access-Control-Allow-Origin", value: "*" }],
      },
    ];
  },
  webpack: (config) => {
    config.plugins.push(
      new webpack.DefinePlugin({
        CESIUM_BASE_URL: JSON.stringify(
          "https://artifact.aleasat.space/cesium",
        ),
      }),
    );
    return config;
  },
  compiler: {
    define: {
      CESIUM_BASE_URL: 
        "https://artifact.aleasat.space/cesium",
    }
  }
};

export default nextConfig;
