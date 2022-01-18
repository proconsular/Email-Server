import {controller, state, Views} from "./state.js";

export class SettingsView {
    constructor() {
        this.labels = []
    }

    init() {
        let view = state.loaded_views[Views.Settings];
        let container = document.createElement("div");
        container.innerHTML = view;

        let list = container.getElementsByClassName("list_body")[0];
        if (list) {
            let receive_rules_card = document.createElement("div");
            receive_rules_card.classList.add("card");
            receive_rules_card.innerText = "Receive Rules";
            receive_rules_card.onclick = async () => {
                await this.load_rules(container)
            }
            list.appendChild(receive_rules_card)
            let account_card = document.createElement("div");
            account_card.classList.add("card");
            account_card.innerText = "Account";
            account_card.onclick = () => {

            }
            list.appendChild(account_card)
        }

        controller.set(container)
    }


    async load_rules(container) {
        let element = document.createElement("div")
        element.innerHTML = `
        <div>
            <div class="rules_list">
                <div class="rule rule_create_form">
                    <div><input class="rule_form_match" type="text" placeholder="Match..."/></div>
                    <div class="rule_labels"><div class="label">none</div></div>
                    <div class="add_labels"><button class="add_label_button">Add Label</button></div>
                    <div class="rule_options"><button class="create_rule_button">Create</button><button class="clear_rule_button">Clear</button></div>
                </div>
            </div>
        </div>
        `
        let form = element.getElementsByClassName("rule_create_form")[0]
        let form_labels_list = form.getElementsByClassName("rule_labels")[0]
        let add_labels_form = form.getElementsByClassName("add_labels")[0]
        let select_label = document.createElement("select")
        state.labels.map(label => {
            let option = document.createElement("option")
            option.text = label.name
            option.value = label.name
            select_label.add(option)
        })
        add_labels_form.prepend(select_label)
        form.getElementsByClassName("add_label_button")[0].onclick = () => {
            if (this.labels.length === 0)
                form_labels_list.innerHTML = ''
            let label = select_label.children[select_label.selectedIndex].value
            this.labels.push(label)
            let label_div = document.createElement("div")
            label_div.classList.add("label")
            label_div.innerText = label
            form_labels_list.appendChild(label_div)
        }
        let match_input = form.getElementsByClassName("rule_form_match")[0]
        form.getElementsByClassName("clear_rule_button")[0].onclick = async () => {
            this.labels = []
            form_labels_list.innerHTML = '<div class="label">none</div>'
        }
        form.getElementsByClassName("create_rule_button")[0].onclick = async () => {
            let rule = {
                match: match_input.value,
                labels: this.labels,
            }
            if (await controller.rules.create(rule)) {
                match_input.value = ""
                this.labels = []
                form_labels_list.innerHTML = ''
                controller.notify("Rule Created")
                await this.load_rules(container)
            } else {
                controller.notify("Failed to Create Rule")
            }
        }

        let subview = container.getElementsByClassName("reader")[0]
        let rules_list = element.getElementsByClassName("rules_list")[0]
        if (await controller.rules.get()) {
            let sorted_rules = state.rules.sort((a, b) => {
                if (a.id < b.id)
                    return 1
                if (a.id > b.id)
                    return -1
                return 0
            })
            for (let rule of sorted_rules) {
                rules_list.appendChild(create_rule(rule))
            }
        }

        subview.innerHTML = '';
        subview.appendChild(element)
    }
}

function create_rule(rule) {
    let rule_div = document.createElement("div")
    rule_div.classList.add("rule")
    rule_div.innerHTML = `
                            <div class="rule_match">${rule.match}</div>
                        `
    let labels_list = document.createElement("div")
    labels_list.classList.add("rule_labels")
    for (let label of rule.labels) {
        let label_div = document.createElement("div")
        label_div.classList.add("label")
        label_div.innerText = label.name
        labels_list.appendChild(label_div)
    }
    rule_div.appendChild(labels_list)
    let rule_options = document.createElement("div")
    rule_options.classList.add("rule_options")
    let delete_button = document.createElement("button")
    delete_button.onclick = async () => {
        let ok = await controller.rules.delete(rule.id)
        if (ok) {
            controller.notify("Rule Deleted")
            rule_div.remove()
        }
    }
    delete_button.innerText = "Delete"
    rule_options.appendChild(delete_button)
    rule_div.appendChild(rule_options)
    return rule_div
}