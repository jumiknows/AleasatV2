import { message, danger, warn, fail, markdown, results } from "danger";
import { execSync } from "child_process";

async function main() {
  // try {
  //   execSync("yarn install --frozen-lockfile")
  //   try {
  //     execSync("yarn lint")
  //   } catch {
  //     warn("Please fix the linting/formatting issues")
  //   }
  // } catch {
  //   warn("Please run `yarn install` and commit the changes")
  // }

  if (!danger.gitlab.mr.title.match(/^(Draft:\s)?ALEA-[0-9]+\s/)) {
    warn(
      "Please start PR title with the OpenProject task ID (ALEA-XXXX) 'ALEA-XXXX add sth'"
    );
  }

  if (!danger.gitlab.mr.description.match(/PP#[0-9]+/)) {
    warn("Please add OpenProject task ID (PP#XXXX) in the PR description");
  }

  const title = danger.gitlab.mr.title.replace(/^(Draft:\s)?ALEA-[0-9]+\s/, "");
  const commitType = [
    "build",
    "chore",
    "ci",
    "docs",
    "feat",
    "fix",
    "perf",
    "refactor",
    "revert",
    "style",
    "test",
  ];

  const mrCommitType = commitType.find((type) => title.startsWith(type));

  const allFiles = danger.git.modified_files
    .concat(danger.git.created_files)
    .concat(danger.git.deleted_files);

  const mopsPr = allFiles
    .filter((file) => !file.startsWith("alea-gsw/apps/alea-san-antonio"))
    .find((file) => file.startsWith("alea-gsw"));

  if (mopsPr && !mrCommitType) {
    warn(
      "Please start the PR title with one of the following types: " +
      commitType.join(", ")
    );
  }

  const labels: string[] = [];

  if (allFiles.find((file) => file.startsWith("alea-fsw/obc-fw"))) {
    labels.push("OBC Firmware");
  }

  if (allFiles.find((file) => file.startsWith("alea-fsw/comms-fw"))) {
    labels.push("Comms Firmware");
  }

  if (allFiles.find((file) => file.startsWith("alea-fsw/python/alea-sim"))) {
    labels.push("ALEASIM");
  }

  if (mopsPr) {
    labels.push("MOPS");
  }

  if (mrCommitType) {
    labels.push(`Type: ${mrCommitType}`);
  }

  const changedPackages: Record<string, string> = {};

  allFiles.forEach((file) => {
    if (file.startsWith("alea-gsw/packages/")) {
      const pkg = file.split("/")[2];
      changedPackages[pkg] = "package";
    } else if (file.startsWith("alea-gsw/apps/")) {
      const app = file.split("/")[2];
      if (app === "alea-san-antonio") {
        labels.push("San Antonio");
        return;
      }
      changedPackages[app] = "app";
    }
  });

  Object.entries(changedPackages).forEach(([name, type]) => {
    labels.push(`${type}/${name}`);
  });

  if (
    Object.keys(changedPackages).includes("ui") ||
    Object.keys(changedPackages).includes("aleasat-site") ||
    Object.keys(changedPackages).includes("aleasat-docs") ||
    Object.keys(changedPackages).includes("dashboard")
  ) {
    labels.push("Barbouts Cut Frontend");
  }
  if (
    Object.keys(changedPackages).filter(
      (key) =>
        key !== "ui" &&
        key !== "aleasat-site" &&
        key !== "aleasat-docs" &&
        key !== "dashboard" &&
        key !== "alea-san-antonio"
    ).length > 0
  ) {
    labels.push("Barbouts Cut Backend");
  }

  if (danger.gitlab.mr.labels)
    for (const label of danger.gitlab.mr.labels) {
      if (
        !labels.includes(label) &&
        (label === "MOPS" ||
          label === "Barbouts Cut Frontend" ||
          label === "Barbouts Cut Backend" ||
          label.startsWith("Type: ") ||
          label.startsWith("package/") ||
          label.startsWith("app/") ||
          label === "San Antonio" ||
          label === "OBC Firmware" ||
          label === "Comms Firmware" ||
          label === "ALEASIM")
      ) {
        await danger.gitlab.utils.removeLabels(label);
      }
    }

  await danger.gitlab.utils.addLabels(...labels);

  if (results.fails.length > 0 || results.warnings.length > 0 || results.messages.length > 0 || results.markdowns.length > 0)
    markdown(
      `**If needed, you can retry the [🔁 \`Run DangerJS CI\` job](https://gitlab.com/alea-2020/software/aleasat-software/-/jobs/${process.env.CI_JOB_ID ?? ""
      }) that generated this comment.**`
    );
}

main();
