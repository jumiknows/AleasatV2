FROM node:22-alpine AS base
RUN corepack enable

FROM base AS base-installer
# Check https://github.com/nodejs/docker-node/tree/b4117f9333da4138b03a546ec926ef50a31506c3#nodealpine to understand why libc6-compat might be needed.
RUN apk add --no-cache libc6-compat

# The web Dockerfile is copy-pasted into our main docs at /docs/handbook/deploying-with-docker.
# Make sure you update this Dockerfile, the Dockerfile in the web workspace and copy that over to Dockerfile in the docs.

# Add lockfile and package.json's of isolated subworkspace
FROM base-installer AS installer
WORKDIR /app
COPY . .

# First install dependencies (as they change less often)
ENV CYPRESS_INSTALL_BINARY="0"

RUN yarn workspaces focus @aleasat/aleasat-docs barbours-cut

# Build the project and its dependencies
RUN yarn turbo run build:types --filter=@aleasat/aleasat-docs...
RUN yarn workspace @aleasat/aleasat-docs build

FROM base AS runner
WORKDIR /app

ENV NODE_ENV production

RUN addgroup --system --gid 1001 nodejs
RUN adduser --system --uid 1001 nodejs
RUN chown nodejs:nodejs /app

RUN mkdir .next
RUN chown nodejs:nodejs .next

COPY --from=installer --chown=nodejs:nodejs /app/apps/aleasat-docs/.next/standalone/apps/aleasat-docs/ ./
COPY --from=installer --chown=nodejs:nodejs /app/apps/aleasat-docs/.next/standalone/node_modules/ ./node_modules
COPY --from=installer --chown=nodejs:nodejs /app/node_modules/@img ./node_modules/@img
COPY --from=installer --chown=nodejs:nodejs /app/apps/aleasat-docs/.next/static ./.next/static
COPY --from=installer --chown=nodejs:nodejs /app/apps/aleasat-docs/.next/cache/fetch-cache ./.next/cache/fetch-cache

USER nodejs

EXPOSE 3000

ENV PORT 3000

CMD ["node", "server.js"]