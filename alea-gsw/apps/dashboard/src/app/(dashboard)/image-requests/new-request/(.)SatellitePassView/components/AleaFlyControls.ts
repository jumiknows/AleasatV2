import type * as three from "three";
import { EventDispatcher, Quaternion, Vector3 } from "three";

class AleaFlyControls extends EventDispatcher {
  moveState: {
    pitchUp: number;
    yawLeft: number;
    rollLeft: number;
    rotate: number;
  };

  lastMouseState: {
    pitchUp: number | undefined;
    yawLeft: number | undefined;
  };

  object: three.PerspectiveCamera;
  domElement: HTMLCanvasElement;
  tmpQuaternion: Quaternion;
  rotationVector: Vector3;
  keydown: (event: KeyboardEvent) => void;
  keyup: (event: KeyboardEvent) => void;
  pointerdown: () => void;
  pointermove: (event: PointerEvent) => void;
  pointerup: () => void;
  update: () => void;
  pointerEnabled: boolean;
  height: number;
  updateHook?: () => void;

  constructor(
    object: THREE.PerspectiveCamera,
    domElement: HTMLCanvasElement,
    height: number,
    updateHook?: () => void,
  ) {
    super();

    this.object = object;
    this.domElement = domElement;
    this.height = height;
    this.updateHook = updateHook;

    this.pointerEnabled = false;

    this.tmpQuaternion = new Quaternion();

    this.moveState = {
      pitchUp: 0,
      yawLeft: 0,
      rollLeft: 0,
      rotate: 0,
    };

    this.lastMouseState = {
      pitchUp: undefined,
      yawLeft: undefined,
    };

    this.rotationVector = new Vector3(0, 0, 0);

    this.keydown = function (event) {
      switch (event.code) {
        case "ControlLeft":
          this.moveState.rotate = 1;
          break;
      }
    };

    this.keyup = function (event) {
      switch (event.code) {
        case "ControlLeft":
          this.moveState.rotate = 0;
          break;
      }
    };

    this.pointermove = function (event) {
      if (!this.pointerEnabled) return;

      const container = this.getContainerDimensions();
      const halfWidth = container.size[0] / 2;
      const halfHeight = container.size[1] / 2;

      const currentYawLeft =
        (event.pageX - container.offset[0] - halfWidth) / halfWidth;
      const currentpitchUp =
        (event.pageY - container.offset[1] - halfHeight) / halfHeight;

      if (!this.lastMouseState.yawLeft || !this.lastMouseState.pitchUp) {
        this.lastMouseState.pitchUp = currentpitchUp;
        this.lastMouseState.yawLeft = currentYawLeft;
        return;
      }

      this.moveState.yawLeft = this.lastMouseState.yawLeft - currentYawLeft;
      this.moveState.pitchUp = this.lastMouseState.pitchUp - currentpitchUp;

      if (this.moveState.rotate) {
        this.moveState.rollLeft =
          (currentpitchUp > 0 ? 1 : -1) * this.moveState.yawLeft +
          (currentYawLeft > 0 ? -1 : 1) * this.moveState.pitchUp;

        this.moveState.yawLeft = 0;
        this.moveState.pitchUp = 0;
      } else {
        this.moveState.rollLeft = 0;
      }

      this.update();
      this.lastMouseState.pitchUp = currentpitchUp;
      this.lastMouseState.yawLeft = currentYawLeft;
    };

    this.pointerdown = function () {
      this.pointerEnabled = true;
      domElement.style.cursor = "grabbing";
    };

    this.pointerup = function () {
      domElement.style.cursor = "grab";
      this.pointerEnabled = false;
      this.lastMouseState.pitchUp = undefined;
      this.lastMouseState.yawLeft = undefined;
      this.moveState.yawLeft = 0;
      this.moveState.pitchUp = 0;
      this.updateHook?.();
    };

    this.update = function () {
      const aovX = Math.tan(0.10485 / 2) * this.height;
      const aovY = Math.tan(0.0782 / 2) * this.height;

      this.tmpQuaternion
        .set(
          Math.atan((aovY * -this.moveState.pitchUp) / this.height) / 3.2,
          Math.atan((aovX * -this.moveState.yawLeft) / this.height) / 3.2,
          this.moveState.rollLeft * 0.5,
          1,
        )
        .normalize();
      this.object.quaternion.multiply(this.tmpQuaternion);
    };

    const _pointermove = this.pointermove.bind(this);
    const _pointerdown = this.pointerdown.bind(this);
    const _pointerup = this.pointerup.bind(this);
    const _keydown = this.keydown.bind(this);
    const _keyup = this.keyup.bind(this);

    this.domElement.addEventListener("pointerdown", _pointerdown);
    this.domElement.addEventListener("pointermove", _pointermove);
    this.domElement.addEventListener("pointerup", _pointerup);

    window.addEventListener("keydown", _keydown);
    window.addEventListener("keyup", _keyup);
  }

  getContainerDimensions() {
    // @ts-expect-error error
    // biome-ignore lint/suspicious/noDoubleEquals: they should
    if (this.domElement != document) {
      return {
        size: [this.domElement.offsetWidth, this.domElement.offsetHeight],
        offset: [
          this.domElement.getBoundingClientRect().x,
          this.domElement.getBoundingClientRect().y,
        ],
      };
    } else {
      return {
        size: [window.innerWidth, window.innerHeight],
        offset: [0, 0],
      };
    }
  }
}

export { AleaFlyControls };
