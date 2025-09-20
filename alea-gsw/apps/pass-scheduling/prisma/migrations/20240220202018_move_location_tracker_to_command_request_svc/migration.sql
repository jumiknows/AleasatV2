/*
  Warnings:

  - You are about to drop the `Location` table. If the table is not empty, all the data it contains will be lost.
  - You are about to drop the `LocationTrack` table. If the table is not empty, all the data it contains will be lost.

*/
-- DropForeignKey
ALTER TABLE "LocationTrack" DROP CONSTRAINT "LocationTrack_currentLocationId_fkey";

-- DropForeignKey
ALTER TABLE "LocationTrack" DROP CONSTRAINT "LocationTrack_finalLocationId_fkey";

-- DropForeignKey
ALTER TABLE "LocationTrack" DROP CONSTRAINT "LocationTrack_initialLocationId_fkey";

-- DropTable
DROP TABLE "Location";

-- DropTable
DROP TABLE "LocationTrack";
