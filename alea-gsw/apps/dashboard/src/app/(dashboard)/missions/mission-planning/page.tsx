"use client";

import { commits, getCommandSpec } from "@aleasat/command-spec";
import { Configuration, MissionApi } from "@aleasat/mission-api";
import {
  DndContext,
  type DragEndEvent,
  KeyboardSensor,
  PointerSensor,
  closestCenter,
  useSensor,
  useSensors,
} from "@dnd-kit/core";
import {
  SortableContext,
  arrayMove,
  sortableKeyboardCoordinates,
  verticalListSortingStrategy,
} from "@dnd-kit/sortable";
import { Button } from "@mui/material";
import Grid2 from "@mui/material/Grid2";
import { useAtom } from "jotai";
import { useSession } from "next-auth/react";
import { env } from "next-runtime-env";
import { useRouter } from "next/navigation";
import React, { useState } from "react";
import CommandInput from "../../../../components/mission-planning/CommandInput";
import MissionObjectDisplay from "../../../../components/mission-planning/MissionObjectDisplay";
import { newMissionCommandsState } from "../../../../components/mission-planning/recoilState";

const { transformedSpec } = getCommandSpec({
  commitHash: commits[1],
})!;

export default function MissionPlanningPage() {
  const router = useRouter();
  const session = useSession();
  const [commands, setCommands] = useAtom(newMissionCommandsState);
  const [lastCommandId, setLastCommandId] = useState(1);

  const sensors = useSensors(
    useSensor(PointerSensor),
    useSensor(KeyboardSensor, {
      coordinateGetter: sortableKeyboardCoordinates,
    }),
  );

  const handleDragEnd = (event: DragEndEvent) => {
    const { active, over } = event;
    if (active.id !== over!.id) {
      setCommands((commands) => {
        const oldIndex = commands.findIndex(
          (command) => command.id === active.id,
        );
        const newIndex = commands.findIndex(
          (command) => command.id === over!.id,
        );
        return arrayMove(commands, oldIndex, newIndex);
      });
    }
  };
  const handleAddCommand = () => {
    setCommands([
      ...commands,
      { id: lastCommandId + 1, commandName: "", commandId: -1, args: [] },
    ]);
    setLastCommandId(lastCommandId + 1);
  };

  const missionVaild = commands.every(
    (command) =>
      command.commandId !== -1 && command.args.every((arg) => arg !== ""),
  );

  const handleSubmitMission = async () => {
    const missionApi = new MissionApi(
      new Configuration({
        accessToken: session.data!.accessToken,
      }),
    );
    const mission = await missionApi.createMission({
      writeMission: {
        fwVersion: commits[1],
        commands: commands.map((command, i) => ({
          sequenceNumber: i + 1,
          commandName: command.commandName,
          commandId: command.commandId,
          arguments: command.args,
        })),
      },
    });
    if (mission) router.push(`/missions/${mission.id}`);
  };

  return (
    <>
      <Button variant="contained" onClick={() => handleAddCommand()}>
        Add Command
      </Button>
      <Grid2 container>
        <Grid2 size={6}>
          <DndContext
            sensors={sensors}
            collisionDetection={closestCenter}
            onDragEnd={handleDragEnd}
          >
            <SortableContext
              items={commands}
              strategy={verticalListSortingStrategy}
            >
              {commands.map((command) => (
                <CommandInput
                  key={command.id}
                  id={command.id}
                  transformedSpec={transformedSpec}
                />
              ))}
            </SortableContext>
          </DndContext>
        </Grid2>
        <Grid2 size={6}>
          <MissionObjectDisplay />
        </Grid2>
      </Grid2>
      <Button
        variant="contained"
        disabled={!missionVaild}
        onClick={async () => await handleSubmitMission()}
      >
        Submit Mission
      </Button>
    </>
  );
}
