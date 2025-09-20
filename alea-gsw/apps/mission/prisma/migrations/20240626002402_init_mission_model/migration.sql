-- CreateTable
CREATE TABLE "Mission" (
    "id" UUID NOT NULL,
    "fwVersion" TEXT NOT NULL,
    "createdAt" TIMESTAMP(3) NOT NULL DEFAULT CURRENT_TIMESTAMP,
    "userId" TEXT NOT NULL,
    "scheduledAt" TIMESTAMP(3),
    "sent" BOOLEAN NOT NULL DEFAULT false,

    CONSTRAINT "Mission_pkey" PRIMARY KEY ("id")
);

-- CreateTable
CREATE TABLE "Command" (
    "id" UUID NOT NULL,
    "missionId" UUID NOT NULL,
    "sequenceNumber" INTEGER NOT NULL,
    "commandId" INTEGER NOT NULL,
    "commandName" TEXT NOT NULL,
    "timeOffset" INTEGER NOT NULL,
    "arguments" TEXT[],
    "sentAt" TIMESTAMP(3),
    "cmdInstId" INTEGER,
    "ranAt" TIMESTAMP(3),
    "responses" TEXT[],

    CONSTRAINT "Command_pkey" PRIMARY KEY ("id")
);

-- CreateIndex
CREATE UNIQUE INDEX "Mission_id_key" ON "Mission"("id");

-- CreateIndex
CREATE UNIQUE INDEX "Command_id_key" ON "Command"("id");

-- CreateIndex
CREATE UNIQUE INDEX "Command_cmdInstId_key" ON "Command"("cmdInstId");

-- AddForeignKey
ALTER TABLE "Command" ADD CONSTRAINT "Command_missionId_fkey" FOREIGN KEY ("missionId") REFERENCES "Mission"("id") ON DELETE RESTRICT ON UPDATE CASCADE;
