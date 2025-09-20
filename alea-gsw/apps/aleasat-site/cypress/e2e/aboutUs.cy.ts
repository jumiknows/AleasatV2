describe.skip("About Page", () => {
	beforeEach(() => {
		cy.visit("/team");
		cy.wait(1000); // wait for tags to be mounted and visible
	});

	it("Should render the scroll down arrow icon", () => {
		cy.get('[data-testid="scroll-down-arrow"]')
			.should("exist")
			.should("be.visible");
	});

	it("Should scroll to team section when scrollDown button is clicked", () => {
		cy.get('[data-testid="scroll-down-arrow"]').click({ force: true });

		cy.get("#next-section")
			.should("exist")
			.should("be.visible");
		cy.get("[data-cy=filter-all]")
			.should("exist")
			.should("be.visible");
		cy.get("[data-cy=filter-captain]")
			.should("exist")
			.should("be.visible");
		cy.get("[data-cy=filter-admin]")
			.should("exist")
			.should("be.visible");
		cy.get("[data-cy=filter-mops]")
			.should("exist")
			.should("be.visible");
		cy.get("[data-cy=filter-advisor]")
			.should("exist")
			.should("be.visible");
	});

	it("Should display Jonathan Ho when MOps is selected and hide when Advisors is selected", () => {
		cy.get("[data-cy=filter-mops]").click();
		cy.get("[data-cy=team-member-name][data-member='Jonathan Ho']")
			.should("exist")
			.should("be.visible");
		cy.get("[data-cy=team-member-role][data-member='Jonathan Ho']")
			.should("exist")
			.should("contain", "SFU MOPS Lead");

		cy.get("[data-cy=filter-advisor]").click();
		cy.get("[data-cy=team-member-name][data-member='Jonathan Ho']")
			.should("not.exist");
	});

	it("Should filter and show all members when 'ALL' is selected", () => {
		cy.get("[data-cy=filter-captain]").click();
		cy.get("[data-cy=team-member-name][data-member='Jen Jon Ma']")
			.should("exist")
			.should("be.visible");
		cy.get("[data-cy=team-member-name][data-member='Jonathan Ho']")
			.should("not.exist");

		cy.get("[data-cy=filter-all]").click();
		cy.get("[data-cy=team-member-name][data-member='Jen Jon Ma']")
			.should("exist")
			.should("be.visible");
		cy.get("[data-cy=team-member-name][data-member='Jonathan Ho']")
			.should("exist")
			.should("be.visible");
		cy.get("[data-cy=team-member-name][data-member='Kasey Chen']")
			.should("exist")
			.should("be.visible");
	});

	it("Should show Captains Jen Jon and Yun and hide others, then swap to Admin for Kasey", () => {
		cy.get("[data-cy=filter-captain]").click();
		cy.get("[data-cy=team-member-name][data-member='Jen Jon Ma']")
			.should("exist")
			.should("be.visible");
		cy.get("[data-cy=team-member-name][data-member='Yun Xing']")
			.should("exist")
			.should("be.visible");

		cy.get("[data-cy=team-member-card][data-member='Jen Jon Ma']").trigger("mouseover");
		cy.get("[data-cy=team-member-role][data-member='Jen Jon Ma']")
			.should("exist")
			.should("contain", "UBC Orbit Captain");

		cy.get("[data-cy=team-member-name][data-member='Jonathan Ho']")
			.should("not.exist");
		cy.get("[data-cy=team-member-name][data-member='Kasey Chen']")
			.should("not.exist");

		cy.get("[data-cy=filter-admin]").scrollIntoView().click();
		cy.get("[data-cy=team-member-name][data-member='Kasey Chen']")
			.should("exist")
			.should("be.visible");
		cy.get("[data-cy=team-member-role][data-member='Kasey Chen']")
			.should("exist")
			.should("contain", "UBC Admin");
	});

	it("Should show social media links and tags on hover", () => {
		cy.get("[data-cy=filter-captain]").click();
		cy.get("[data-cy=team-member-card][data-member='Yun Xing']").trigger("mouseover");

		cy.get("[data-cy=team-member-hover-overlay][data-member='Yun Xing']")
			.should("exist")
			.should("be.visible");
		cy.get("[data-cy=github-link][data-member='Yun Xing']")
			.should("exist")
			.should("be.visible");
		cy.get("[data-cy=linkedin-link][data-member='Yun Xing']")
			.should("exist")
			.should("be.visible");
		cy.get("[data-cy=member-tag][data-tag='UBC']")
			.should("exist")
			.should("be.visible");
	});

	it("Should open popup modal when clicking on person in mobile view", () => {
		cy.viewport("iphone-6");
		cy.get("[data-cy=filter-advisor]").click();

		cy.get("[data-cy=team-member-card][data-member='Julian Mentasti']").click();

		cy.get("[data-cy=member-modal][data-member='Julian Mentasti']")
			.should("exist")
			.should("be.visible");
		cy.get("[data-cy=modal-tag][data-tag='UBC']")
			.should("exist")
			.should("be.visible");
		cy.get("[data-cy=modal-tag][data-tag='Graduated']")
			.should("exist")
			.should("be.visible");
		cy.get("[data-cy=modal-tag][data-tag='CPSC']")
			.should("exist")
			.should("be.visible");

		cy.get("[data-cy=modal-close-button]").click();
	});

	it("Should maintain functionality even after switching teams constantly", () => {
		cy.get("[data-cy=filter-all]").click();
		cy.wait(150);
		cy.get("[data-cy=filter-captain]").click();
		cy.wait(150);
		cy.get("[data-cy=filter-admin]").click();
		cy.wait(150);
		cy.get("[data-cy=filter-mops]").click();
		cy.wait(150);
		cy.get("[data-cy=filter-advisor]").click();
		cy.wait(150);
		cy.get("[data-cy=filter-comms]").click();
		cy.wait(150);
		cy.get("[data-cy=filter-all]").click();

		cy.get("[data-cy=team-member-name][data-member='Jen Jon Ma']")
			.should("exist")
			.should("be.visible");
		cy.get("[data-cy=team-members-container]")
			.should("exist")
			.should("be.visible");
	});
});