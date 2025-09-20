import React from "react";
import Hero from "../../../src/components/home/Hero";

describe("<Footer />", () => {
  it("renders", () => {
    cy.mount(<Hero />);
  });
});
