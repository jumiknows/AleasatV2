FROM node:22-alpine AS base
RUN corepack enable

FROM base AS base-installer
# Check https://github.com/nodejs/docker-node/tree/b4117f9333da4138b03a546ec926ef50a31506c3#nodealpine to understand why libc6-compat might be needed.
RUN apk add --no-cache libc6-compat

# The web Dockerfile is copy-pasted into our main docs at /docs/handbook/deploying-with-docker.
# Make sure you update this Dockerfile, the Dockerfile in the web workspace and copy that over to Dockerfile in the docs.

FROM base-installer AS builder
# Set working directory
WORKDIR /app
RUN yarn global add turbo
COPY . .
RUN turbo prune @aleasat/dashboard --docker

# Add lockfile and package.json's of isolated subworkspace
FROM base-installer AS installer
WORKDIR /app

# First install dependencies (as they change less often)
ENV CYPRESS_INSTALL_BINARY="0"

COPY --from=builder /app/out/json/ .
COPY --from=builder /app/out/yarn.lock ./yarn.lock
COPY --from=builder /app/out/full/.yarn/patches .yarn/patches
RUN yarn install

# Build the project and its dependencies
COPY --from=builder /app/out/full/ .
COPY turbo.json turbo.json

RUN yarn turbo run build:types --filter=@aleasat/dashboard...
RUN yarn workspace @aleasat/dashboard build

FROM base AS runner
WORKDIR /app

ENV NODE_ENV production

RUN addgroup --system --gid 1001 nodejs
RUN adduser --system --uid 1001 nodejs
RUN chown nodejs:nodejs /app

RUN mkdir .next
RUN chown nodejs:nodejs .next

COPY --from=installer --chown=nodejs:nodejs /app/apps/dashboard/.next/standalone/apps/dashboard/ ./
COPY --from=installer --chown=nodejs:nodejs /app/apps/dashboard/.next/standalone/node_modules/ ./node_modules
COPY --from=installer --chown=nodejs:nodejs /app/node_modules/@img ./node_modules/@img
COPY --from=installer --chown=nodejs:nodejs /app/apps/dashboard/.next/static ./.next/static

USER nodejs

EXPOSE 3000

ENV PORT 3000

CMD ["node", "server.js"]