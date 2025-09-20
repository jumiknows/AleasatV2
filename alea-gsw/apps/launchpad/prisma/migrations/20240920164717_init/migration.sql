-- CreateTable
CREATE TABLE "Launchpad" (
    "id" UUID NOT NULL,
    "name" TEXT NOT NULL,
    "node" TEXT NOT NULL,
    "port" TEXT NOT NULL,
    "serviceName" TEXT NOT NULL,
    "fwCommit" TEXT NOT NULL,

    CONSTRAINT "Launchpad_pkey" PRIMARY KEY ("id")
);

-- CreateTable
CREATE TABLE "UserSession" (
    "id" UUID NOT NULL,
    "userId" TEXT NOT NULL,
    "startAt" TIMESTAMP(3) NOT NULL DEFAULT CURRENT_TIMESTAMP,
    "endAt" TIMESTAMP(3) NOT NULL,
    "launchpadId" UUID NOT NULL,

    CONSTRAINT "UserSession_pkey" PRIMARY KEY ("id")
);

-- CreateIndex
CREATE UNIQUE INDEX "Launchpad_id_key" ON "Launchpad"("id");

-- CreateIndex
CREATE UNIQUE INDEX "UserSession_id_key" ON "UserSession"("id");

-- CreateIndex
CREATE UNIQUE INDEX "UserSession_launchpadId_key" ON "UserSession"("launchpadId");

-- AddForeignKey
ALTER TABLE "UserSession" ADD CONSTRAINT "UserSession_launchpadId_fkey" FOREIGN KEY ("launchpadId") REFERENCES "Launchpad"("id") ON DELETE RESTRICT ON UPDATE CASCADE;
