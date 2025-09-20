-- CreateTable
CREATE TABLE "StateElement" (
    "id" UUID NOT NULL,
    "stateElement" JSONB NOT NULL,
    "tle" TEXT,
    "epoch" BIGINT NOT NULL,

    CONSTRAINT "StateElement_pkey" PRIMARY KEY ("id")
);

-- CreateIndex
CREATE UNIQUE INDEX "StateElement_id_key" ON "StateElement"("id");

-- CreateIndex
CREATE UNIQUE INDEX "StateElement_epoch_stateElement_key" ON "StateElement"("epoch", "stateElement");
