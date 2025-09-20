-- CreateEnum
CREATE TYPE "TransferType" AS ENUM ('upload', 'download');

-- CreateTable
CREATE TABLE "Transfer" (
    "id" UUID NOT NULL,
    "commandId" UUID NOT NULL,
    "type" "TransferType" NOT NULL,
    "priority" INTEGER NOT NULL,
    "passId" UUID NOT NULL,

    CONSTRAINT "Transfer_pkey" PRIMARY KEY ("id")
);

-- CreateTable
CREATE TABLE "PredictedRiseSet" (
    "id" UUID NOT NULL,
    "rise" INTEGER NOT NULL,
    "set" INTEGER NOT NULL,
    "stateElementId" UUID NOT NULL,
    "previousPassId" UUID,

    CONSTRAINT "PredictedRiseSet_pkey" PRIMARY KEY ("id")
);

-- CreateTable
CREATE TABLE "ActualRiseSet" (
    "id" UUID NOT NULL,
    "rise" INTEGER NOT NULL,
    "set" INTEGER NOT NULL,
    "passId" UUID NOT NULL,

    CONSTRAINT "ActualRiseSet_pkey" PRIMARY KEY ("id")
);

-- CreateTable
CREATE TABLE "Pass" (
    "id" UUID NOT NULL,
    "predictedRiseSetId" UUID NOT NULL,
    "groundStationId" INTEGER NOT NULL,
    "memoryBudget" INTEGER NOT NULL DEFAULT 0,
    "memoryAllocated" INTEGER NOT NULL DEFAULT 0,
    "previousPassId" UUID,

    CONSTRAINT "Pass_pkey" PRIMARY KEY ("id")
);

-- CreateTable
CREATE TABLE "GroundStation" (
    "id" SERIAL NOT NULL,
    "lat" DOUBLE PRECISION NOT NULL,
    "lng" DOUBLE PRECISION NOT NULL,
    "description" TEXT NOT NULL,
    "name" TEXT NOT NULL,
    "minElevation" DOUBLE PRECISION NOT NULL,
    "ownerId" TEXT,
    "aleasat" BOOLEAN NOT NULL DEFAULT false,

    CONSTRAINT "GroundStation_pkey" PRIMARY KEY ("id")
);

-- CreateIndex
CREATE UNIQUE INDEX "Transfer_id_key" ON "Transfer"("id");

-- CreateIndex
CREATE UNIQUE INDEX "PredictedRiseSet_id_key" ON "PredictedRiseSet"("id");

-- CreateIndex
CREATE UNIQUE INDEX "ActualRiseSet_id_key" ON "ActualRiseSet"("id");

-- CreateIndex
CREATE UNIQUE INDEX "ActualRiseSet_passId_key" ON "ActualRiseSet"("passId");

-- CreateIndex
CREATE UNIQUE INDEX "Pass_id_key" ON "Pass"("id");

-- CreateIndex
CREATE UNIQUE INDEX "Pass_predictedRiseSetId_key" ON "Pass"("predictedRiseSetId");

-- CreateIndex
CREATE UNIQUE INDEX "Pass_previousPassId_key" ON "Pass"("previousPassId");

-- CreateIndex
CREATE UNIQUE INDEX "GroundStation_id_key" ON "GroundStation"("id");

-- AddForeignKey
ALTER TABLE "Transfer" ADD CONSTRAINT "Transfer_passId_fkey" FOREIGN KEY ("passId") REFERENCES "Pass"("id") ON DELETE RESTRICT ON UPDATE CASCADE;

-- AddForeignKey
ALTER TABLE "PredictedRiseSet" ADD CONSTRAINT "PredictedRiseSet_previousPassId_fkey" FOREIGN KEY ("previousPassId") REFERENCES "Pass"("id") ON DELETE SET NULL ON UPDATE CASCADE;

-- AddForeignKey
ALTER TABLE "ActualRiseSet" ADD CONSTRAINT "ActualRiseSet_passId_fkey" FOREIGN KEY ("passId") REFERENCES "Pass"("id") ON DELETE RESTRICT ON UPDATE CASCADE;

-- AddForeignKey
ALTER TABLE "Pass" ADD CONSTRAINT "Pass_predictedRiseSetId_fkey" FOREIGN KEY ("predictedRiseSetId") REFERENCES "PredictedRiseSet"("id") ON DELETE RESTRICT ON UPDATE CASCADE;

-- AddForeignKey
ALTER TABLE "Pass" ADD CONSTRAINT "Pass_groundStationId_fkey" FOREIGN KEY ("groundStationId") REFERENCES "GroundStation"("id") ON DELETE RESTRICT ON UPDATE CASCADE;

-- AddForeignKey
ALTER TABLE "Pass" ADD CONSTRAINT "Pass_previousPassId_fkey" FOREIGN KEY ("previousPassId") REFERENCES "Pass"("id") ON DELETE SET NULL ON UPDATE CASCADE;
