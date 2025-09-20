FROM node:22-alpine AS base
RUN corepack enable

FROM base AS base-installer
# Check https://github.com/nodejs/docker-node/tree/b4117f9333da4138b03a546ec926ef50a31506c3#nodealpine to understand why libc6-compat might be needed.
RUN apk add --no-cache libc6-compat

# The web Dockerfile is copy-pasted into our main docs at /docs/handbook/deploying-with-docker.
# Make sure you update this Dockerfile, the Dockerfile in the web workspace and copy that over to Dockerfile in the docs.

ENV PUPPETEER_EXECUTABLE_PATH /usr/bin/google-chrome
ENV PUPPETEER_SKIP_CHROMIUM_DOWNLOAD true

FROM base-installer AS builder
# Set working directory
WORKDIR /app
RUN yarn global add turbo
COPY . .
RUN turbo prune @aleasat/template --docker

# Add lockfile and package.json's of isolated subworkspace
FROM base-installer AS installer
WORKDIR /app

# First install dependencies (as they change less often)

COPY --from=builder /app/out/json/ .
COPY --from=builder /app/out/yarn.lock ./yarn.lock
RUN yarn install

# Build the project and its dependencies
COPY --from=builder /app/out/full/ .
COPY turbo.json turbo.json

RUN yarn turbo run build --filter=@aleasat/template...

FROM base-installer AS prod-deps
WORKDIR /app
COPY --from=builder /app/out/json/ .
COPY --from=builder /app/out/yarn.lock ./yarn.lock
RUN yarn workspaces focus --all --production

FROM base AS runner
WORKDIR /app

# Don't run production as root
RUN addgroup --system --gid 1001 nodejs
RUN adduser --system --uid 1001 nodejs
RUN chown nodejs:nodejs /app
COPY --chown=nodejs:nodejs --from=installer /app/package.json .
COPY --chown=nodejs:nodejs --from=installer /app/yarn.lock .
COPY --chown=nodejs:nodejs --from=installer /app/.yarnrc.yml .
COPY --chown=nodejs:nodejs --from=installer /app/.yarn/releases .yarn/releases
RUN chown nodejs:nodejs /app/.yarn/
COPY --chown=nodejs:nodejs --from=prod-deps /app/node_modules node_modules
COPY --chown=nodejs:nodejs --from=installer /app/packages packages
COPY --chown=nodejs:nodejs --from=installer /app/apps apps
COPY --chown=nodejs:nodejs --from=installer /app/node_modules/.prisma/client node_modules/.prisma/client

USER nodejs

ENV PORT=3000

WORKDIR /app/apps/template

CMD node --enable-source-maps --no-warnings=ExperimentalWarning --loader @opentelemetry/instrumentation/hook.mjs ./dist/server.js