"use server";

import { auth } from "@/auth";
import {
  Configuration,
  type ImageRequest,
  ImageRequestApi,
  type ImageRequestVaildationError,
} from "@aleasat/api-lib";
import type { AxiosResponse } from "axios";
import { revalidatePath } from "next/cache";

export async function updateWorkingImageRequest(
  _prevState: {
    loading: boolean;
    success: boolean;
    imageRequestId?: string;
  },
  imageRequestState: {
    name: string;
    description: string;
    satelliteLocationLat: number;
    satelliteLocationLng: number;
    satelliteLocationT: string;
    satelliteRotationX: number;
    satelliteRotationY: number;
    satelliteRotationZ: number;
    AROgroundStationId: number;
    AROgroundStationPassRise: string;
    AROgroundStationPassSet: string;
  },
): Promise<
  | {
      loading: boolean;
      success: false;
      imageRequestVaildationError: ImageRequestVaildationError;
      nextError?: undefined;
    }
  | {
      loading: boolean;
      success: false;
      nextError: string;
      imageRequestVaildationError?: undefined;
    }
  | {
      loading: boolean;
      success: true;
      imageRequestId: string;
    }
> {
  const session = await auth();

  if (!session)
    return {
      loading: false,
      success: false,
      nextError: "Unauthorized",
    };

  if (
    !imageRequestState.satelliteLocationT ||
    !imageRequestState.name ||
    !imageRequestState.description
  )
    return {
      loading: false,
      success: false,
      nextError: "Missing Fields",
    };

  const AROgroundStationId =
    imageRequestState.AROgroundStationId !== -1
      ? imageRequestState.AROgroundStationId
      : null;
  const AROgroundStationPassRise =
    imageRequestState.AROgroundStationId !== -1
      ? imageRequestState.AROgroundStationPassRise
      : null;
  const AROgroundStationPassSet =
    imageRequestState.AROgroundStationId !== -1
      ? imageRequestState.AROgroundStationPassSet
      : null;

  let res: AxiosResponse<ImageRequest, any>;
  try {
    res = await new ImageRequestApi(
      new Configuration({
        accessToken: session.accessToken,
      }),
    ).addImageRequest(
      {
        createImageRequestParameter: {
          name: imageRequestState.name,
          description: imageRequestState.description,
          satelliteLocationLat: imageRequestState.satelliteLocationLat,
          satelliteLocationLng: imageRequestState.satelliteLocationLng,
          satelliteLocationT: imageRequestState.satelliteLocationT,
          satelliteRotationX: imageRequestState.satelliteRotationX,
          satelliteRotationY: imageRequestState.satelliteRotationY,
          satelliteRotationZ: imageRequestState.satelliteRotationZ,
          AROgroundStationId,
          AROgroundStationPassRise,
          AROgroundStationPassSet,
        },
      },
      {
        validateStatus: (status) =>
          (status >= 200 && status < 300) || status === 400,
        headers: { Authorization: "Bearer " + session.accessToken },
      },
    );
  } catch (e) {
    console.error(e);
    return {
      loading: false,
      success: false,
      nextError: "Internal Server Error",
    };
  }

  if (res.status === 400)
    return {
      loading: false,
      success: false,
      imageRequestVaildationError:
        res.data as unknown as ImageRequestVaildationError,
    };

  revalidatePath("/my-image");
  return { loading: false, success: true, imageRequestId: res.data.id };
}
