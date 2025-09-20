import type { ReadMissionStatusEnum } from "@aleasat/mission-api";
import { Chip } from "@mui/material";

const statusColor = (status: ReadMissionStatusEnum) => {
  switch (status) {
    case "created":
      return "info";
    case "scheduled":
      return "warning";
    case "sent":
      return "success";
    case "error":
      return "error";
  }
};

export default function MissionStatusChip({
  status,
}: {
  status: ReadMissionStatusEnum;
}) {
  return <Chip label={status} color={statusColor(status)} variant="filled" />;
}
