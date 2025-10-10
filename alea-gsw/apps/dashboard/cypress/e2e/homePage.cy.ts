describe("Home Page", () => {
  beforeEach(() => {
    cy.visit("/");
  });

  it("should contain main call to attention", () => {
    cy.get("h3").should(
      "contain",
      "Get ready to capture images straight from space!",
    );
    cy.get("h6").should(
      "contain",
      "Send a request to our satellite to receive your first image",
    );
  });

  it("should contain text in event of no image", () => {
    cy.get("h4").should(
      "contain",
      "OOPS It looks like you have not requested any images yet.",
    );
  });

  it("should contain telemetry", () => {
    cy.get("h3").should("contain", "Telemetry and Data");
  });

  it("should contain footer", () => {
    cy.get("p").should(
      "contain",
      "© 2025 UBC Orbit Design Team & SFU Satellite Design Team",
    );
  });
});
