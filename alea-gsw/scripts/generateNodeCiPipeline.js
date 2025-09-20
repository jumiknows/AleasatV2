import { exec } from 'child_process';
import fs from 'fs';
import path from 'path';
import integrationTests from '../.gitlab/workflows/integration-tests.js';

const apps = fs.readdirSync('apps').filter((app) =>
  fs.existsSync(`apps/${app}/package.json`)
);

const appsDeps = {}

apps.forEach((app) => {
  const packageJson = JSON.parse(fs.readFileSync(`apps/${app}/package.json`, 'utf8'))
  appsDeps[app] = Object.keys({ ...packageJson.dependencies, ...packageJson.devDependencies })
})

const webApps = apps.filter((app) => appsDeps[app].includes('react'))
const nodeApps = apps.filter((app) => !webApps.includes(app))

const usedDeps = {}

const generateAleaDeps = (app, indent) => {
  let AleaDeps = ""
  appsDeps[app].forEach((dep) => {
    if (dep.startsWith("@aleasat/")) {
      AleaDeps += `${indent}- alea-gsw/packages/${dep.slice(9)}/**/*\n`
      usedDeps[dep] = true
    }
  })
  return AleaDeps.slice(0, -1)
}

let pipeline = ``
nodeApps.forEach((app) => {
  pipeline += `"${app} Unit Test":
  rules:
    - if: $CI_PIPELINE_SOURCE == "merge_request_event"
      changes:
        paths:
          - alea-gsw/apps/${app}/**/*
${generateAleaDeps(app, "          ")}
          - alea-gsw/.gitlab/workflows/unit-test.yml
          - alea-gsw/.gitlab/workflows/publish.yml
          - alea-gsw/.dockerignore${integrationTests[app] ? `
"${app} Integration Test":
  ${Object.entries(integrationTests[app].variables).length > 0 ? `variables:` : ""}
${Object.entries(integrationTests[app].variables).map(([key, value]) => `    ${key}: ${value}`).join("\n")}
  services:
${integrationTests[app].services.map((value) =>
    `    - name: ${value.name}
${Object.entries(value).slice(1).map(([key, value]) => `      ${key}: ${value}`).join("\n")}`
  ).join("\n")}
  rules:
    - if: $CI_PIPELINE_SOURCE == "merge_request_event"
      changes:
        paths:
          - alea-gsw/apps/${app}/**/*
${generateAleaDeps(app, "          ")}
          - alea-gsw/.gitlab/workflows/integration-test.yml
          - alea-gsw/.gitlab/workflows/publish.yml
          - alea-gsw/.dockerignore` : ""}
"${app} Build and Push Docker Image":
  rules:
    - if: $CI_COMMIT_BRANCH == "dev"
      changes:
        - alea-gsw/apps/${app}/**/*
${generateAleaDeps(app, "        ")}
        - alea-gsw/.gitlab/workflows/publish.yml
        - alea-gsw/.dockerignore
"${app} Build Docker Image":${integrationTests[app] ? `
  needs:
    - ${app} Unit Test
    - ${app} Integration Test
    - Build Types and Test` : ""}
  rules:
    - if: $CI_PIPELINE_SOURCE == "merge_request_event"
      changes:
        paths:
          - alea-gsw/apps/${app}/**/*
${generateAleaDeps(app, "          ")}
          - alea-gsw/.gitlab/workflows/publish.yml
          - alea-gsw/.dockerignore
"${app} Docker Image Smoke Test":
  rules:
    - if: $CI_PIPELINE_SOURCE == "merge_request_event"
      changes:
        paths:
          - alea-gsw/apps/${app}/**/*
${generateAleaDeps(app, "          ")}
          - alea-gsw/.gitlab/workflows/publish.yml
          - alea-gsw/.dockerignore
"Deploy ${app} to staging":
  needs:
    - "${app} Build and Push Docker Image"
  rules:
    - if: $CI_COMMIT_BRANCH == "dev"
      changes:
        - alea-gsw/apps/${app}/**/*
${generateAleaDeps(app, "        ")}
        - alea-gsw/.gitlab/workflows/publish.yml
        - alea-gsw/.dockerignore
`
})

pipeline += `"aleasat-site E2E Test":
  rules:
    - if: $CI_PIPELINE_SOURCE == "merge_request_event"
      changes:
        paths:
          - alea-gsw/apps/aleasat-site/**/*
${generateAleaDeps("aleasat-site", "          ")}
          - alea-gsw/.gitlab/workflows/e2e-test.yml
          - alea-gsw/.gitlab/workflows/publish-aleasat-site.yml
          - alea-gsw/.dockerignore
"Publish aleasat-site Container":
  rules:
    - if: $CI_COMMIT_BRANCH == "dev"
      changes:
        - alea-gsw/apps/aleasat-site/**/*
${generateAleaDeps("aleasat-site", "        ")}
        - alea-gsw/.gitlab/workflows/publish-aleasat-site.yml
        - alea-gsw/.dockerignore
"Deploy aleasat-site to staging":
  needs:
    - "Publish aleasat-site Container"
  rules:
    - if: $CI_COMMIT_BRANCH == "dev"
      changes:
        - alea-gsw/apps/aleasat-site/**/*
${generateAleaDeps("aleasat-site", "        ")}
        - alea-gsw/.gitlab/workflows/publish-aleasat-site.yml
        - alea-gsw/.dockerignore
"Deploy aleasat-site to production":
  needs:
    - "Deploy aleasat-site to staging"
  rules:
    - if: $CI_COMMIT_BRANCH == "dev"
      changes:
        - alea-gsw/apps/aleasat-site/**/*
${generateAleaDeps("aleasat-site", "        ")}
        - alea-gsw/.gitlab/workflows/publish-aleasat-site.yml
        - alea-gsw/.dockerignore
"Publish aleasat-site review Container":
  rules:
    - if: $CI_PIPELINE_SOURCE == "merge_request_event"
      changes:
        paths:
          - alea-gsw/apps/aleasat-site/**/*
${generateAleaDeps("aleasat-site", "          ")}
          - alea-gsw/.gitlab/workflows/publish-aleasat-site.yml
          - alea-gsw/.dockerignore
Deploy aleasat-site:
  rules:
    - if: $CI_PIPELINE_SOURCE == "merge_request_event"
      changes:
        paths:
          - alea-gsw/apps/aleasat-site/**/*
${generateAleaDeps("aleasat-site", "          ")}
          - alea-gsw/.gitlab/workflows/publish-aleasat-site.yml
          - alea-gsw/.dockerignore
`

webApps.filter((app) => app !== "aleasat-site" && app !== "aleasat-docs").forEach((app) => {
  pipeline += `"${app} E2E Test":
  rules:
    - if: $CI_PIPELINE_SOURCE == "merge_request_event"
      changes:
        paths:
          - alea-gsw/apps/${app}/**/*
${generateAleaDeps(app, "          ")}
          - alea-gsw/.gitlab/workflows/e2e-test.yml
          - alea-gsw/.gitlab/workflows/publish-website.yml
          - alea-gsw/.dockerignore
"Publish ${app} Container":
  rules:
    - if: $CI_COMMIT_BRANCH == "dev"
      changes:
        - alea-gsw/apps/${app}/**/*
${generateAleaDeps(app, "        ")}
        - alea-gsw/.gitlab/workflows/publish-website.yml
        - alea-gsw/.dockerignore
"Deploy ${app} to staging":
  needs:
    - "Publish ${app} Container"
  rules:
    - if: $CI_COMMIT_BRANCH == "dev"
      changes:
        - alea-gsw/apps/${app}/**/*
${generateAleaDeps(app, "        ")}
        - alea-gsw/.gitlab/workflows/publish-website.yml
        - alea-gsw/.dockerignore
"Build ${app} Container":
  rules:
    - if: $CI_PIPELINE_SOURCE == "merge_request_event"
      changes:
        paths:
          - alea-gsw/apps/${app}/**/*
${generateAleaDeps(app, "          ")}
          - alea-gsw/.gitlab/workflows/publish-website.yml
          - alea-gsw/.dockerignore
`
})

pipeline += `include:\n`
nodeApps.forEach((app) => {
  pipeline += `  - local: "alea-gsw/.gitlab/workflows/unit-test.yml"
    inputs:
      package: "${app}"${integrationTests[app] ? `
  - local: "alea-gsw/.gitlab/workflows/integration-test.yml"
    inputs:
      package: "${app}"` : ""}
  - local: "alea-gsw/.gitlab/workflows/publish.yml"
    inputs:
      package: "${app}"
  - local: "alea-gsw/.gitlab/workflows/update-deployment.yml"
    inputs:
      package: "${app}"
      environment: staging
      tag: $CI_COMMIT_SHA
`
})

pipeline += `
  - local: "alea-gsw/.gitlab/workflows/e2e-test.yml"
    inputs:
      package: "aleasat-site"
  - local: "alea-gsw/.gitlab/workflows/publish-aleasat-site.yml"
  - local: "alea-gsw/.gitlab/workflows/update-deployment.yml"
    inputs:
      package: "aleasat-site"
      environment: staging
      tag: $CI_COMMIT_SHA
  - local: "alea-gsw/.gitlab/workflows/publish-aleasat-docs.yml"
`

webApps.filter((app) => app !== "aleasat-site" && app !== "aleasat-docs").forEach((app) => {
  pipeline += `  - local: "alea-gsw/.gitlab/workflows/e2e-test.yml"
    inputs:
      package: "${app}"
  - local: "alea-gsw/.gitlab/workflows/publish-website.yml"
    inputs:
      package: "${app}"
  - local: "alea-gsw/.gitlab/workflows/update-deployment.yml"
    inputs:
      package: "${app}"
      environment: staging
      tag: $CI_COMMIT_SHA
`
})

const oldPipeline = fs.readFileSync('.gitlab/workflows/node-pipeline.yml', "utf-8")

if (oldPipeline === pipeline) {
  process.exit(0)
}

console.log("Detected changes in dependencies, updating CI pipeline")

fs.writeFileSync('.gitlab/workflows/node-pipeline.yml', pipeline)

exec(`git add ${path.resolve(
  import.meta.dirname, '../.gitlab/workflows/node-pipeline.yml'
)
  } `, (error, stdout, stderr) => {
    if (error||stderr) console.error(`error: ${error ?? stderr} `);
    exec('git commit -m "ci: update node pipeline"', (error, stdout, stderr) => {
      if (error||stderr) console.error(`error: ${error ?? stderr} `);
    })
  })
