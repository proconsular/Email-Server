import {controller, state, Views} from "./state.js";

export class ComposeView {

    init() {
        let view = state.loaded_views[Views.Compose];
        let container = document.createElement("div");
        container.setAttribute("id", "view_compose")
        container.innerHTML = view;
        controller.set(container);
        document.getElementById("compose_send_button").onclick = async () => {
            let recipients_input = document.getElementById("compose_to_input")
            let subject_input = document.getElementById("compose_subject_input")
            let body_input = document.getElementById("compose_body_input")
            let recipients = ""
            let subject = ""
            let body = ""
            if (recipients_input)
                recipients = recipients_input.value
            if (subject_input)
                subject = subject_input.value
            if (body_input)
                body = body_input.value
            if (recipients.length > 0) {
                let ok = await controller.send_email(recipients, subject, body)
                if (ok) {
                    controller.set_view(Views.Main)
                }
                controller.notify(ok ? "Mail Sent" : "Failed to Send")
            }
        }
        document.getElementById("compose_cancel_button").onclick = () => {
            controller.set_view(Views.Main);
        }
    }
}