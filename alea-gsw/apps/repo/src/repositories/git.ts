import fs from "fs/promises";
import simpleGit, { ResetMode } from "simple-git";
import constants from "../constants.js";

await fs.mkdir("cache", { recursive: true });

const cacheDir = await fs.readdir("cache");

if (!cacheDir.includes("aleasat-software")) {
  await simpleGit().clone(
    `https://${constants.aleaSwRepoDeployToken.user}:${constants.aleaSwRepoDeployToken.password}@gitlab.com/alea-2020/software/aleasat-software`,
    "cache/aleasat-software",
  );
}

const git = simpleGit("cache/aleasat-software");
await git.fetch();
await git.reset(ResetMode.HARD, ["origin/dev"]);

export const updateRepoInterval = setInterval(
  async () => {
    await git.fetch();
    await git.reset(ResetMode.HARD, ["origin/dev"]);
  },
  1000 * 60 * 5,
);

export default git;
