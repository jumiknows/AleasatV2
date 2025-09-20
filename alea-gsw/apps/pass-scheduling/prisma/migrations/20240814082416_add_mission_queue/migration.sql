-- CreateEnum
CREATE TYPE "MissionStatus" AS ENUM ('queued', 'expired', 'scheduled');

-- CreateTable
CREATE TABLE "MissionQueue" (
    "id" UUID NOT NULL,
    "queueAt" TIMESTAMP(3) NOT NULL DEFAULT CURRENT_TIMESTAMP,
    "missionId" UUID NOT NULL,

    CONSTRAINT "MissionQueue_pkey" PRIMARY KEY ("id")
);

-- CreateTable
CREATE TABLE "MissionSchedule" (
    "id" UUID NOT NULL,
    "initialOverheadStart" TIMESTAMP(3) NOT NULL,
    "missionStart" TIMESTAMP(3) NOT NULL,
    "missionEnd" TIMESTAMP(3) NOT NULL,
    "directOverheadEnd" TIMESTAMP(3) NOT NULL,
    "indirectOverheadEnd" TIMESTAMP(3) NOT NULL,
    "missionId" UUID,

    CONSTRAINT "MissionSchedule_pkey" PRIMARY KEY ("id")
);

-- CreateTable
CREATE TABLE "MissionScheduleSpace" (
    "id" UUID NOT NULL,
    "start" TIMESTAMP(3) NOT NULL,
    "end" TIMESTAMP(3),
    "duration" INTEGER NOT NULL,

    CONSTRAINT "MissionScheduleSpace_pkey" PRIMARY KEY ("id")
);

-- CreateTable
CREATE TABLE "CommandSchedule" (
    "id" UUID NOT NULL,
    "start" TIMESTAMP(3) NOT NULL,
    "end" TIMESTAMP(3) NOT NULL,
    "overheadEnd" TIMESTAMP(3) NOT NULL,
    "commandId" UUID NOT NULL,

    CONSTRAINT "CommandSchedule_pkey" PRIMARY KEY ("id")
);

-- CreateTable
CREATE TABLE "Mission" (
    "id" UUID NOT NULL,
    "status" "MissionStatus" NOT NULL DEFAULT 'queued',
    "scheduledAt" TIMESTAMP(3),

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
    "scheduledAt" TIMESTAMP(3),

    CONSTRAINT "Command_pkey" PRIMARY KEY ("id")
);

-- CreateIndex
CREATE UNIQUE INDEX "MissionQueue_id_key" ON "MissionQueue"("id");

-- CreateIndex
CREATE UNIQUE INDEX "MissionQueue_missionId_key" ON "MissionQueue"("missionId");

-- CreateIndex
CREATE UNIQUE INDEX "MissionSchedule_id_key" ON "MissionSchedule"("id");

-- CreateIndex
CREATE UNIQUE INDEX "MissionSchedule_missionId_key" ON "MissionSchedule"("missionId");

-- CreateIndex
CREATE UNIQUE INDEX "MissionScheduleSpace_id_key" ON "MissionScheduleSpace"("id");

-- CreateIndex
CREATE UNIQUE INDEX "CommandSchedule_id_key" ON "CommandSchedule"("id");

-- CreateIndex
CREATE UNIQUE INDEX "CommandSchedule_commandId_key" ON "CommandSchedule"("commandId");

-- CreateIndex
CREATE UNIQUE INDEX "Mission_id_key" ON "Mission"("id");

-- CreateIndex
CREATE UNIQUE INDEX "Command_id_key" ON "Command"("id");

-- AddForeignKey
ALTER TABLE "MissionQueue" ADD CONSTRAINT "MissionQueue_missionId_fkey" FOREIGN KEY ("missionId") REFERENCES "Mission"("id") ON DELETE RESTRICT ON UPDATE CASCADE;

-- AddForeignKey
ALTER TABLE "MissionSchedule" ADD CONSTRAINT "MissionSchedule_missionId_fkey" FOREIGN KEY ("missionId") REFERENCES "Mission"("id") ON DELETE SET NULL ON UPDATE CASCADE;

-- AddForeignKey
ALTER TABLE "CommandSchedule" ADD CONSTRAINT "CommandSchedule_commandId_fkey" FOREIGN KEY ("commandId") REFERENCES "Command"("id") ON DELETE RESTRICT ON UPDATE CASCADE;

-- AddForeignKey
ALTER TABLE "Command" ADD CONSTRAINT "Command_missionId_fkey" FOREIGN KEY ("missionId") REFERENCES "Mission"("id") ON DELETE RESTRICT ON UPDATE CASCADE;
