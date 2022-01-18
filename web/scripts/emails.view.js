import {controller, state, Views} from "./state.js";
import {create_card} from "./shared.js";

export class MainView {
    constructor() {
        this.selected = ''
        this.email_elements = {}
    }

    async init() {
        let view = state.loaded_views[Views.Main];
        let container = document.createElement("div");
        container.setAttribute("id", Views.Main);
        container.innerHTML = view;
        let ok = await controller.get_emails()
        if (ok) {
            let list = container.getElementsByClassName("list_body")[0];
            if (list) {
                if (state.emails == null) {
                    let e = document.createElement("div")
                    e.innerText = "No Emails"
                    e.style.width = "350px";
                    e.style.padding = "12px"
                    e.style.textAlign = "center"
                    e.style.fontSize = "24px"
                    list.append(e)
                } else {
                    for (let i = state.emails.length - 1; i >= 0; i--) {
                        let email = state.emails[i]
                        let sub = email.subject || "";
                        let card = create_card({
                            subject: sub === "NULL" ? "(No Subject)" : sub,
                            from: email.sender,
                            summary: ''
                        })
                        if (email.sender === `${state.login.username}@drade.io`) {
                            card.getElementsByClassName("card")[0].setAttribute("class", "card red")
                        }
                        let read = false
                        for (let label of email.labels) {
                            if (label.name === "read")
                                read = true
                        }
                        if (!read) {
                            card.getElementsByClassName("card")[0].classList.add("blue")
                        }
                        card.onclick = async () => {
                            await controller.get_email(email["message-id"])
                            this.selected = email["message-id"]
                            this.set_body(email.sender, email.subject, state.selected_email)

                        }
                        card.getElementsByClassName("ui_delete")[0].onclick = (event) => {
                            this.delete(email["message-id"])
                            event.stopPropagation()
                        }
                        this.email_elements[email["message-id"]] = card;
                        list.prepend(card)
                    }
                }
            }
            controller.set(container)
            document.getElementById("email_compose_button").onclick = () => {
                controller.set_view(Views.Compose);
            }
        } else {
            controller.clear_session()
            controller.set_view(Views.Login)
        }
    }

    set_body(from_c, subject_c, content_c) {
        let from = document.querySelector("#email_reader .email_from")
        if (from) {
            from.innerText = from_c;
        }
        let subject = document.querySelector("#email_reader .email_subject")
        if (subject) {
            subject.innerText = subject_c;
        }
        let body = document.querySelector("#email_reader .email_body")
        if (body) {
            body.innerHTML = content_c
        }
    }

    async delete(name) {
        if (await controller.delete_email(name)) {
            this.email_elements[name].remove()
            if (this.selected === name) {
                this.set_body("", "", "")
            }
        }
    }
}

