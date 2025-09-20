import ApiDocs from "@/components/ApiDocs";
import fs from "fs/promises";
import { unstable_cache } from "next/cache.js";

const getApiSpecs = unstable_cache(async () => {
  const appsDir = await fs.readdir("..");
  return await Promise.all(
    appsDir.map(
      async (app) =>
        await fs
          .readFile(`../${app}/api/openapi.json`, "utf-8")
          .catch(() => null),
    ),
  ).then((apiSpecs) => apiSpecs.filter((spec) => spec !== null));
});

export default async function Docs() {
  const apiSpecs = await getApiSpecs();

  return (
    <>
      <ApiDocs apiSpecs={apiSpecs} />
    </>
  );
}
