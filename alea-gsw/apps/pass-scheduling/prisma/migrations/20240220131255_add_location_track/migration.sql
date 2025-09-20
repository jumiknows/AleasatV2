-- CreateTable
CREATE TABLE "Location" (
    "id" UUID NOT NULL,
    "lat" DOUBLE PRECISION NOT NULL,
    "lng" DOUBLE PRECISION NOT NULL,
    "alt" DOUBLE PRECISION NOT NULL,
    "t" TIMESTAMP(3) NOT NULL,
    "stateElementId" UUID,

    CONSTRAINT "Location_pkey" PRIMARY KEY ("id")
);

-- CreateTable
CREATE TABLE "LocationTrack" (
    "id" UUID NOT NULL,
    "initialLocationId" UUID NOT NULL,
    "currentLocationId" UUID NOT NULL,
    "finalLocationId" UUID,

    CONSTRAINT "LocationTrack_pkey" PRIMARY KEY ("id")
);

-- CreateIndex
CREATE UNIQUE INDEX "Location_id_key" ON "Location"("id");

-- CreateIndex
CREATE UNIQUE INDEX "LocationTrack_id_key" ON "LocationTrack"("id");

-- CreateIndex
CREATE UNIQUE INDEX "LocationTrack_initialLocationId_key" ON "LocationTrack"("initialLocationId");

-- CreateIndex
CREATE UNIQUE INDEX "LocationTrack_currentLocationId_key" ON "LocationTrack"("currentLocationId");

-- CreateIndex
CREATE UNIQUE INDEX "LocationTrack_finalLocationId_key" ON "LocationTrack"("finalLocationId");

-- AddForeignKey
ALTER TABLE "LocationTrack" ADD CONSTRAINT "LocationTrack_initialLocationId_fkey" FOREIGN KEY ("initialLocationId") REFERENCES "Location"("id") ON DELETE RESTRICT ON UPDATE CASCADE;

-- AddForeignKey
ALTER TABLE "LocationTrack" ADD CONSTRAINT "LocationTrack_currentLocationId_fkey" FOREIGN KEY ("currentLocationId") REFERENCES "Location"("id") ON DELETE RESTRICT ON UPDATE CASCADE;

-- AddForeignKey
ALTER TABLE "LocationTrack" ADD CONSTRAINT "LocationTrack_finalLocationId_fkey" FOREIGN KEY ("finalLocationId") REFERENCES "Location"("id") ON DELETE SET NULL ON UPDATE CASCADE;
