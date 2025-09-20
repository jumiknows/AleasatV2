import { updateStore } from "../repositories/orbitPropagationStore.js";

const webhooksNewGmatOrbitPropagation = async (): Promise<void> => {
  updateStore();
};

export default { webhooksNewGmatOrbitPropagation };
