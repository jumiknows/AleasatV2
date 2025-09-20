"use server";

import { auth } from "@/auth";
import {
  Configuration,
  GroundStationApi,
  type GroundStationCreate,
} from "@aleasat/pass-scheduling-api";
import { revalidatePath } from "next/cache";

export async function addGroundStation(
  _prevState: {
    errorMessage: string;
    success: boolean;
  },
  formData: FormData,
): Promise<{
  errorMessage: string;
  success: boolean;
}> {
  const session = await auth();

  if (session === null) {
    return {
      errorMessage: "Unauthorized, please login before creating",
      success: false,
    };
  }

  const groundStationCreate: GroundStationCreate = {
    name: formData.get("name") as string,
    description: formData.get("description") as string,
    lat: parseFloat(formData.get("latitude") as string),
    lng: parseFloat(formData.get("longitude") as string),
    minElevation: parseFloat(formData.get("minElevation") as string),
    ownerId: session.user.id,
  };

  try {
    await new GroundStationApi(
      new Configuration({
        accessToken: session.accessToken,
      }),
    ).addGroundStation({
      groundStationCreate,
    });
  } catch {
    return {
      errorMessage: "Error please try again.",
      success: false,
    };
  }

  revalidatePath("/settings/ground-station");
  return {
    errorMessage: "",
    success: true,
  };
}

export async function editGroundStation(
  groundStationId: number,
  _prevState: {
    errorMessage: string;
    success: boolean;
  },
  formData: FormData,
): Promise<{
  errorMessage: string;
  success: boolean;
}> {
  const session = await auth();

  if (session === null) {
    return {
      errorMessage: "Unauthorized, please login before creating",
      success: false,
    };
  }

  try {
    await new GroundStationApi(
      new Configuration({
        accessToken: session.accessToken,
      }),
    ).patchGroundStation({
      groundStationId: groundStationId.toString(),
      groundStationOptional: {
        name: formData.get("name") as string,
        description: formData.get("description") as string,
        lat: parseFloat(formData.get("latitude") as string),
        lng: parseFloat(formData.get("longitude") as string),
        minElevation: parseFloat(formData.get("minElevation") as string),
      },
    });
  } catch {
    return {
      errorMessage: "Error please try again.",
      success: false,
    };
  }

  revalidatePath("/settings/ground-station");
  return {
    errorMessage: "",
    success: true,
  };
}

export async function deleteGroundStation(
  groundStationId: number,
  _prevState: {
    errorMessage: string;
    success: boolean;
  },
  _formData: FormData,
): Promise<{
  errorMessage: string;
  success: boolean;
}> {
  const session = await auth();

  if (session === null) {
    return {
      errorMessage: "Unauthorized, please login before creating",
      success: false,
    };
  }

  try {
    await new GroundStationApi(
      new Configuration({
        accessToken: session.accessToken,
      }),
    ).deleteGroundStation({
      groundStationId: groundStationId.toString(),
    });
  } catch {
    return {
      errorMessage: "Error please try again.",
      success: false,
    };
  }

  revalidatePath("/settings/ground-station");
  return {
    errorMessage: "",
    success: true,
  };
}
