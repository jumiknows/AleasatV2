import { updateRepoInterval } from "../../src/repositories/git.js";

after(() => {
  clearInterval(updateRepoInterval);
});
