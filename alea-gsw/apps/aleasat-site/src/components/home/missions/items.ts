export interface MissionItem {
  icon: string;
  title: string;
  description: string;
}

export const missionItems: MissionItem[] = [
  {
    icon: "disaster.png",
    title: "Disaster Relief Satellite",
    description:
      "To provide amateur radio community a training satellite to help in disaster mitigation and relief via on-demand satellite imagery.",
  },
  {
    icon: "educate.png",
    title: "Educational Outreach",
    description:
      "To support educational outreach activities for university and secondary school students.",
  },
  {
    icon: "research.png",
    title: "Image Processing Research",
    description:
      "To investigate the performance of different on-board image post-processing algorithms.",
  },
];
